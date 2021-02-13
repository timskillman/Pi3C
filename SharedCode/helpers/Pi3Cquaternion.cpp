#include "Pi3Cquaternion.h"

//void Pi3Cquaternion::quaternionY(Pi3Cmatrix* mtx, float y, float w)
//{
//	float(*q)[3] = mtx->m;
//	float y2 = 1.f - y*y*2.f;
//	float wy = w*y*2.f;
//	q[Pi3Cmatrix::m00] = y2; q[Pi3Cmatrix::m10] = 0; q[Pi3Cmatrix::m20] = -wy;
//	q[Pi3Cmatrix::m01] = 0; q[Pi3Cmatrix::m11] = 1.f; q[Pi3Cmatrix::m21] = 0;
//	q[Pi3Cmatrix::m02] = wy; q[Pi3Cmatrix::m12] = 0; q[Pi3Cmatrix::m22] = y2;
//}
//
//void Pi3Cquaternion::quaternionZ(Pi3Cmatrix* mtx, float z, float w)
//{
//	float(*q)[3] = mtx->m;
//	float z2 = 1.f - z*z*2.f;
//	float wz = w*z*2.f;
//	q[Pi3Cmatrix::m00] = z2; q[Pi3Cmatrix::m10] = wz; q[Pi3Cmatrix::m20] = 0;
//	q[Pi3Cmatrix::m01] = -wz; q[Pi3Cmatrix::m11] = z2; q[Pi3Cmatrix::m21] = 0;
//	q[Pi3Cmatrix::m02] = 0; q[Pi3Cmatrix::m12] = 0; q[Pi3Cmatrix::m22] = 1.f;
//}

//void Pi3Cquaternion::quaternionXm(Pi3Cmatrix* mtxq, Pi3Cmatrix* mtxp, float a)
//{
//	//Setup X rotation and multiply with p matrix
//	float* mtq = mtxq.matrix;
//	float* mtp = mtxp.matrix;
//	float x = sinf(a);
//	float x2 = 1.f - x*x*2.f;
//	float wx = cosf(a) * x*2.f;
//	mtq[Pi3Cmatrix::m00] = mtp[Pi3Cmatrix::m00];
//	mtq[Pi3Cmatrix::m10] = mtp[Pi3Cmatrix::m10] * x2 + mtp[Pi3Cmatrix::m20] * -wx;
//	mtq[Pi3Cmatrix::m20] = mtp[Pi3Cmatrix::m10] * wx + mtp[Pi3Cmatrix::m20] * x2;
//	mtq[Pi3Cmatrix::m01] = mtp[Pi3Cmatrix::m01];
//	mtq[Pi3Cmatrix::m11] = mtp[Pi3Cmatrix::m11] * x2 + mtp[Pi3Cmatrix::m21] * -wx;
//	mtq[Pi3Cmatrix::m21] = mtp[Pi3Cmatrix::m11] * wx + mtp[Pi3Cmatrix::m21] * x2;
//	mtq[Pi3Cmatrix::m02] = mtp[Pi3Cmatrix::m02];
//	mtq[Pi3Cmatrix::m12] = mtp[Pi3Cmatrix::m12] * x2 + mtp[Pi3Cmatrix::m22] * -wx;
//	mtq[Pi3Cmatrix::m22] = mtp[Pi3Cmatrix::m12] * wx + mtp[Pi3Cmatrix::m22] * x2;
//}

void Pi3Cquaternion::quaternionXm(Pi3Cmatrix& matx, float ang)
{
	//Setup X rotation and multiply with p matrix
	float* mtx = matx.matrix;
	float xx = sinf(ang);
	float x2 = 1.f - xx * xx * 2.f;
	float wx = cosf(ang) * xx * 2.f;
	float a = mtx[Pi3Cmatrix::m01] * x2 + mtx[Pi3Cmatrix::m02] * -wx;
	mtx[Pi3Cmatrix::m02] = mtx[Pi3Cmatrix::m01] * wx + mtx[Pi3Cmatrix::m02] * x2;
	mtx[Pi3Cmatrix::m01] = a;
	a = mtx[Pi3Cmatrix::m11] * x2 + mtx[Pi3Cmatrix::m12] * -wx;
	mtx[Pi3Cmatrix::m12] = mtx[Pi3Cmatrix::m11] * wx + mtx[Pi3Cmatrix::m12] * x2;
	mtx[Pi3Cmatrix::m11] = a;
	a = mtx[Pi3Cmatrix::m21] * x2 + mtx[Pi3Cmatrix::m22] * -wx;
	mtx[Pi3Cmatrix::m22] = mtx[Pi3Cmatrix::m21] * wx + mtx[Pi3Cmatrix::m22] * x2;
	mtx[Pi3Cmatrix::m21] = a;
	matx.identity = false;
}

void Pi3Cquaternion::quaternionYm(Pi3Cmatrix& matx, float ang)
{
	//Setup Y rotation and multiply with p matrix
	float* mtx = matx.matrix;
	float yy = sinf(ang);
	float y2 = 1.f - yy * yy * 2.f;
	float wy = cosf(ang) * yy * 2.f;
	float a = mtx[Pi3Cmatrix::m00] * y2 + mtx[Pi3Cmatrix::m02] * wy;
	mtx[Pi3Cmatrix::m02] = mtx[Pi3Cmatrix::m00] * -wy + mtx[Pi3Cmatrix::m02] * y2;
	mtx[Pi3Cmatrix::m00] = a;
	a = mtx[Pi3Cmatrix::m10] * y2 + mtx[Pi3Cmatrix::m12] * wy;
	mtx[Pi3Cmatrix::m12] = mtx[Pi3Cmatrix::m10] * -wy + mtx[Pi3Cmatrix::m12] * y2;
	mtx[Pi3Cmatrix::m10] = a;
	a = mtx[Pi3Cmatrix::m20] * y2 + mtx[Pi3Cmatrix::m22] * wy;
	mtx[Pi3Cmatrix::m22] = mtx[Pi3Cmatrix::m20] * -wy + mtx[Pi3Cmatrix::m22] * y2;
	mtx[Pi3Cmatrix::m20] = a;
	matx.identity = false;
}

void Pi3Cquaternion::quaternionZm(Pi3Cmatrix& matx, float ang)
{
	//Setup Z rotation and multiply with p matrix
	float* mtx = matx.matrix;
	float zz = sinf(ang);
	float z2 = 1.f - zz * zz * 2.f;
	float wz = cosf(ang) * zz * 2.f;
	float a = mtx[Pi3Cmatrix::m00] * z2 + mtx[Pi3Cmatrix::m01] * -wz;
	mtx[Pi3Cmatrix::m01] = mtx[Pi3Cmatrix::m00] * wz + mtx[Pi3Cmatrix::m01] * z2;
	mtx[Pi3Cmatrix::m00] = a;
	a = mtx[Pi3Cmatrix::m10] * z2 + mtx[Pi3Cmatrix::m11] * -wz;
	mtx[Pi3Cmatrix::m11] = mtx[Pi3Cmatrix::m10] * wz + mtx[Pi3Cmatrix::m11] * z2;
	mtx[Pi3Cmatrix::m10] = a;
	a = mtx[Pi3Cmatrix::m20] * z2 + mtx[Pi3Cmatrix::m21] * -wz;
	mtx[Pi3Cmatrix::m21] = mtx[Pi3Cmatrix::m20] * wz + mtx[Pi3Cmatrix::m21] * z2;
	mtx[Pi3Cmatrix::m20] = a;
	matx.identity = false;
}

static void quaternion(Pi3Cmatrix& matx, float x, float y, float z, float w)
{
	/* Setup matrix from quaternion */
	float x2 = x * x * 2.f;
	float y2 = y * y * 2.f;
	float z2 = z * z * 2.f;
	float xy = x * y * 2.f;
	float xz = x * z * 2.f;
	float yz = y * z * 2.f;
	float wx = w * x * 2.f;
	float wy = w * y * 2.f;
	float wz = w * z * 2.f;

	float* mtx = matx.matrix;
	mtx[Pi3Cmatrix::m00] = 1.f - y2 - z2;
	mtx[Pi3Cmatrix::m01] = xy + wz;
	mtx[Pi3Cmatrix::m02] = xz - wy;

	mtx[Pi3Cmatrix::m10] = xy - wz;
	mtx[Pi3Cmatrix::m11] = 1.f - x2 - z2;
	mtx[Pi3Cmatrix::m12] = yz + wx;

	mtx[Pi3Cmatrix::m20] = xz + wy;
	mtx[Pi3Cmatrix::m21] = yz - wx;
	mtx[Pi3Cmatrix::m22] = 1.f - x2 - y2;
	matx.identity = false;
}

void Pi3Cquaternion::matrixToQuat(Pi3Cmatrix& matx)
{
	/* Extracts the X,Y,Z,W quaternion from a matrix */

	float* mtx = matx.matrix;
	float trace = mtx[Pi3Cmatrix::m00] + mtx[Pi3Cmatrix::m11] + mtx[Pi3Cmatrix::m22];
	if (trace > 0) {
		float s = 0.5f / sqrtf(trace + 1.0f);
		x = (mtx[Pi3Cmatrix::m12] - mtx[Pi3Cmatrix::m21]) * s;
		y = (mtx[Pi3Cmatrix::m20] - mtx[Pi3Cmatrix::m02]) * s;
		z = (mtx[Pi3Cmatrix::m01] - mtx[Pi3Cmatrix::m10]) * s;
		w = 0.25f / s;
	}
	else {
		if ((mtx[Pi3Cmatrix::m00] > mtx[Pi3Cmatrix::m11]) && (mtx[Pi3Cmatrix::m00] > mtx[Pi3Cmatrix::m22])) {
			float s = 0.5f / sqrtf(1.0f + mtx[Pi3Cmatrix::m00] - mtx[Pi3Cmatrix::m11] - mtx[Pi3Cmatrix::m22]);
			x = 0.25f / s;
			y = (mtx[Pi3Cmatrix::m10] + mtx[Pi3Cmatrix::m01]) * s;
			z = (mtx[Pi3Cmatrix::m20] + mtx[Pi3Cmatrix::m02]) * s;
			w = (mtx[Pi3Cmatrix::m12] - mtx[Pi3Cmatrix::m21]) * s;
		}
		else if (mtx[Pi3Cmatrix::m11] > mtx[Pi3Cmatrix::m22]) {
			float s = 0.5f / sqrtf(1.0f + mtx[Pi3Cmatrix::m11] - mtx[Pi3Cmatrix::m00] - mtx[Pi3Cmatrix::m22]);
			x = (mtx[Pi3Cmatrix::m01] + mtx[Pi3Cmatrix::m10]) * s;
			y = 0.25f / s;
			z = (mtx[Pi3Cmatrix::m12] + mtx[Pi3Cmatrix::m21]) * s;
			w = (mtx[Pi3Cmatrix::m20] - mtx[Pi3Cmatrix::m02]) * s;
		}
		else {
			float s = 0.5f / sqrtf(1.0f + mtx[Pi3Cmatrix::m22] - mtx[Pi3Cmatrix::m00] - mtx[Pi3Cmatrix::m11]);
			x = (mtx[Pi3Cmatrix::m20] + mtx[Pi3Cmatrix::m02]) * s;
			y = (mtx[Pi3Cmatrix::m21] + mtx[Pi3Cmatrix::m12]) * s;
			z = 0.25f / s;
			w = (mtx[Pi3Cmatrix::m01] - mtx[Pi3Cmatrix::m10]) * s;
		}
	}

	//Normalise output and clear errors
	float sq = 1.f / sqrtf(x * x + y * y + z * z + w * w);
	x = x * sq;
	y = y * sq;
	z = z * sq;
	w = w * sq;
}

Pi3Cmatrix Pi3Cquaternion::toMatrix()
{
	Pi3Cmatrix tmtx;
	quaternion(tmtx, x, y, z, w);
	return tmtx;
}

void Pi3Cquaternion::toMatrix16floats(float* mtx)
{
	Pi3Cmatrix tmtx;
	quaternion(tmtx, x, y, z, w);
	mtx[0] = tmtx.matrix[Pi3Cmatrix::m00];
	mtx[1] = tmtx.matrix[Pi3Cmatrix::m01];
	mtx[2] = tmtx.matrix[Pi3Cmatrix::m02];
	mtx[4] = tmtx.matrix[Pi3Cmatrix::m10];
	mtx[5] = tmtx.matrix[Pi3Cmatrix::m11];
	mtx[6] = tmtx.matrix[Pi3Cmatrix::m12];
	mtx[8] = tmtx.matrix[Pi3Cmatrix::m20];
	mtx[9] = tmtx.matrix[Pi3Cmatrix::m21];
	mtx[10] = tmtx.matrix[Pi3Cmatrix::m22];
}

void Pi3Cquaternion::rotateAboutX(float a)
{
	Pi3Cmatrix tmtx;
	quaternion(tmtx, x, y, z, w);
	quaternionXm(tmtx, a / 2.f);
	matrixToQuat(tmtx);
}

void Pi3Cquaternion::rotateAboutY(float a)
{
	Pi3Cmatrix tmtx;
	quaternion(tmtx, x, y, z, w);
	quaternionYm(tmtx, a / 2.f);
	matrixToQuat(tmtx);
}

void Pi3Cquaternion::rotateAboutZ(float a)
{
	Pi3Cmatrix tmtx;
	quaternion(tmtx, x, y, z, w);
	quaternionZm(tmtx, a / 2.f);
	matrixToQuat(tmtx);
}

void Pi3Cquaternion::rotateAboutAxis(float x, float y, float z, float a)
{
	float factor = sinf(a / 2.0f);
	float xx = x * factor;
	float yy = y * factor;
	float zz = z * factor;
	float w = cosf(a / 2.0f);

	Pi3Cmatrix mp, mq;
	quaternion(mp, xx, yy, zz, w);
	quaternion(mq, x, y, z, w);
	Pi3Cmatrix mr = mq * mp;
	matrixToQuat(mr);
}

void Pi3Cquaternion::combineQuat(Pi3Cquaternion q)
{
	Pi3Cmatrix mp, mq;
	quaternion(mp, q.x, q.y, q.z, q.w);
	quaternion(mq, x, y, z, w);
	Pi3Cmatrix mr = mq * mp;
	matrixToQuat(mr);
}

