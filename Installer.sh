#!/bin/sh

CONFIG=/boot/config.txt
ASK_TO_REBOOT=0

# Install packages
PACKAGES="libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-net-dev"
apt-get update
apt-get upgrade -y
apt-get install $PACKAGES -y

is_pione() {
   if grep -q "^Revision\s*:\s*00[0-9a-fA-F][0-9a-fA-F]$" /proc/cpuinfo; then
      return 0
   elif grep -q "^Revision\s*:\s*[ 123][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]0[0-36][0-9a-fA-F]$" /proc/cpuinfo ; then
      return 0
   else
      return 1
   fi
}

is_pitwo() {
   grep -q "^Revision\s*:\s*[ 123][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]04[0-9a-fA-F]$" /proc/cpuinfo
   return $?
}

is_pizero() {
   grep -q "^Revision\s*:\s*[ 123][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]0[9cC][0-9a-fA-F]$" /proc/cpuinfo
   return $?
}

is_pifour() {
   grep -q "^Revision\s*:\s*[ 123][0-9a-fA-F][0-9a-fA-F]3[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]$" /proc/cpuinfo
   return $?
}

#Enable GL driver support (Pi4 / other Pi's) - pinched from Raspi-config settings
if is_pifour ; then
        if is_pifour ; then
          if ! grep -q -E "^dtoverlay=vc4-fkms-v3d" $CONFIG; then
            ASK_TO_REBOOT=1
          fi
        else
          if ! sed -n "/\[pi4\]/,/\[/ !p" $CONFIG | grep -q "^dtoverlay=vc4-fkms-v3d" ; then
            ASK_TO_REBOOT=1
          fi
        fi
        sed $CONFIG -i -e "s/^dtoverlay=vc4-kms-v3d/#dtoverlay=vc4-kms-v3d/g"
        sed $CONFIG -i -e "s/^#dtoverlay=vc4-fkms-v3d/dtoverlay=vc4-fkms-v3d/g"
        if ! sed -n "/\[pi4\]/,/\[/ !p" $CONFIG | grep -q "^dtoverlay=vc4-fkms-v3d" ; then
          printf "[all]\ndtoverlay=vc4-fkms-v3d\n" >> $CONFIG
        fi
        STATUS="The fake KMS GL driver is enabled."
else
	if ! sed -n "/\[pi4\]/,/\[/ !p" $CONFIG | grep -q "^dtoverlay=vc4-kms-v3d" ; then
          ASK_TO_REBOOT=1
        fi
        sed $CONFIG -i -e "s/^dtoverlay=vc4-fkms-v3d/#dtoverlay=vc4-fkms-v3d/g"
        sed $CONFIG -i -e "s/^#dtoverlay=vc4-kms-v3d/dtoverlay=vc4-kms-v3d/g"
        if ! sed -n "/\[pi4\]/,/\[/ !p" $CONFIG | grep -q "^dtoverlay=vc4-kms-v3d" ; then
          printf "[all]\ndtoverlay=vc4-kms-v3d\n" >> $CONFIG
        fi
        STATUS="The full KMS GL driver is enabled."
fi

# If a line containing "gpu_mem" exists
if grep -Fq "gpu_mem" $CONFIG
then
	# Replace the line
	echo "Modifying gpu_mem"
	sed -i "/gpu_mem/c\gpu_mem=64" $CONFIG
else
	# Create the definition
	echo "gpu_mem not defined. Creating definition"
	echo "gpu_mem=64" >> $CONFIG
fi

  if [ $ASK_TO_REBOOT -eq 1 ]; then
    whiptail --yesno "Would you like to reboot now?" 20 60 2
    if [ $? -eq 0 ]; then # yes
      sync
      reboot
    fi
  fi

echo "Install complete, rebooting."
reboot