namespace Actors
{
	DWORD_PTR Uworld;
	DWORD_PTR Rootcomp;
	DWORD_PTR Localplayer;
	DWORD_PTR PlayerController;
	DWORD_PTR LocalPawn;
	DWORD_PTR PlayerState;
	DWORD_PTR Persistentlevel;
	uintptr_t PlayerCameraManager;
	DWORD_PTR GameState;
	DWORD_PTR PlayerArray;
	DWORD_PTR LocalPlayers;
	uintptr_t Gameinstance;
}

#define PI 3.14159265358979323846f

struct _MATRIX
{
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float m[4][4];
	};

	_MATRIX() : _11(1.f), _12(0.f), _13(0.f), _14(0.f),
		_21(0.f), _22(1.f), _23(0.f), _24(0.f),
		_31(0.f), _32(0.f), _33(1.f), _34(0.f),
		_41(0.f), _42(0.f), _43(0.f), _44(1.f) {}
};

_MATRIX Matrix(Vector3 euler, Vector3 Vec4, Vector3 origin = Vector3(0, 0, 0))
{
	const float radPitch = euler.x * PI / 180.f;
	const float radYaw = euler.y * PI / 180.f;
	const float radRoll = euler.z * PI / 180.f;
	const float SP = sinf(radPitch);
	const float CP = cosf(radPitch);
	const float SY = sinf(radYaw);
	const float CY = cosf(radYaw);
	const float SR = sinf(radRoll);
	const float CR = cosf(radRoll);

	_MATRIX mat;

	mat.m[0][0] = CP * CY;
	mat.m[0][1] = CP * SY;
	mat.m[0][2] = SP;
	mat.m[0][3] = 0.f;

	mat.m[1][0] = SR * SP * CY - CR * SY;
	mat.m[1][1] = SR * SP * SY + CR * CY;
	mat.m[1][2] = -SR * CP;
	mat.m[1][3] = 0.f;

	mat.m[2][0] = -(CR * SP * CY + SR * SY);
	mat.m[2][1] = CY * SR - CR * SP * SY;
	mat.m[2][2] = CR * CP;
	mat.m[2][3] = 0.f;

	mat.m[3][0] = origin.x;
	mat.m[3][1] = origin.y;
	mat.m[3][2] = origin.z;
	mat.m[3][3] = 1.f;

	return mat;
}

struct Camera
{
	float FieldOfView;
	Vector3 Rotation;
	Vector3 Location;
}; Camera vCamera;

Camera GetCamera(__int64 a1)
{
	Camera LocalCamera;
	__int64 v1;
    v1 = read<__int64>(Actors::Localplayer + 0xd0);
	const __int64 v9 = read<__int64>(v1 + 0x8);
	LocalCamera.FieldOfView = 80.f / (read<double>(v9 + 0x7F0) / 1.19f);
	LocalCamera.Rotation.x = read<double>(v9 + 0x9C0);
	LocalCamera.Rotation.y = read<double>(a1 + 0x148);
	uint64_t PointerLoc = read<uint64_t>(Actors::Uworld + 0x110);
	LocalCamera.Location = read<Vector3>(PointerLoc);
	return LocalCamera;
}
Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	Camera vCamera = GetCamera(Actors::Rootcomp);
	vCamera.Rotation.x = (asin(vCamera.Rotation.x)) * (180.0 / M_PI);
	_MATRIX tempMatrix = Matrix(vCamera.Rotation, Vector3(0, 0, 0));
	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.z < 1.f) vTransformed.z = 1.f;
	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}