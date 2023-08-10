#pragma once
#include "vector.h"

class CUserCmd
{
public:
	int command_number;

	// Token: 0x04001284 RID: 4740
	int tick_count;

	// Token: 0x04001285 RID: 4741
	QAngle viewangles;

	// Token: 0x04001286 RID: 4742
	Vector move;

	// Token: 0x04001287 RID: 4743
	uint64_t buttons;

	// Token: 0x04001288 RID: 4744
	uint64_t lastbuttons;

	// Token: 0x04001289 RID: 4745
	int random_seed;

	// Token: 0x0400128A RID: 4746
	short mousedx;
	// Token: 0x0400128B RID: 4747
	short mousedy;

	// Token: 0x0400128C RID: 4748
	short mousewheel;

	// Token: 0x0400128D RID: 4749
	int SelectionIndex;

	// Token: 0x0400128E RID: 4750
	int SelectionSubIndex;

	// Token: 0x0400128F RID: 4751
	bool hasbeenpredicted;

	// Token: 0x04001290 RID: 4752
	QAngle cursor_origin;

	// Token: 0x04001291 RID: 4753
	QAngle cursor_aim;

	// Token: 0x04001292 RID: 4754
	QAngle viewpos;

	// Token: 0x04001293 RID: 4755
	bool usingcontroller;
};

class Vector4D
{
public:
	Vector4D()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		q = 0.0f;
	}
private:
	float x, y, z, q;
};


enum Buttons : uint64_t
{
	Forward = 1,
	Back = 2,
	Left = 4,
	Right = 8,
	Jump = 16,
	Duck = 32,
	Run = 64,
	Walk = 128,
	PrimaryAttack = 256,
	SecondaryAttack = 512,
	Reload = 1024,
	Grenade = 2048,
	Drop = 4096,
	Use = 8192,
	Flashlight = 16384,
	View = 32768,
	Zoom = 65536,
	Menu = 131072,
	Score = 262144,
	Chat = 524288,
	Voice = 1048576,
	SlotNext = 2097152,
	SlotPrev = 4194304,
	Slot0 = 4294967296,
	Slot1 = 8388608,
	Slot2 = 16777216,
	Slot3 = 33554432,
	Slot4 = 67108864,
	Slot5 = 134217728,
	Slot6 = 268435456,
	Slot7 = 536870912,
	Slot8 = 1073741824,
	Slot9 = 2147483648,
};


enum HitboxType : int
{
	Sphere,
	Capulse,
	Box
};

class player_hitbox
{
public:
	Vector mins()
	{
		return *(Vector*)(this + 32);
	}
	Vector maxs()
	{
		return *(Vector*)(this + 44);
	}

	const char* pName()
	{
		return *(const char**)(this + 0x38);
	}

	HitboxType type()
	{
		return *(HitboxType*)(this + 98);
	}

	float radius()
	{
		return *(float*)(this + 80);
	}

	bool is_hitbox()
	{
		return *(bool*)(this + 99);
	}
};

class TraceResult
{
public:
	Vector m_StartPos; //0x0000
	Vector m_EndPos; //0x000C
	Vector m_HitPos; //0x0018
	Vector m_Normal; //0x0024
	float m_Fraction; //0x0030
	int32_t m_Entity; //0x0034
	bool m_StartedInSolid; //0x0038
	int32_t m_PhysicsHandle; //0x0039
	int32_t m_PhysicsShapeHandle; //0x003D
	int32_t m_Handle; //0x0041
	int32_t m_Hitbox; //0x0045
	char pad_0049[100]; //0x0049
}; //Size: 0x02A5


class TraceRequest
{
public:
	Vector m_vecStart; //0x0000
	Vector4D m_vecStartRot; //0x000C
	Vector m_vecEnd; //0x0018
	Vector2D m_vecEndRot; //0x0024
	Vector m_vecMins; //0x0030
	Vector m_vecMaxs; //0x003C
	bool m_bWorld; //0x0048
	bool m_bEntities; //0x0049
	char pad_004A[6]; //0x004A
	int64_t m_nMask; //0x0050
	void* m_pEnt; //0x0058
	char pad_0060[8]; //0x0060
	int32_t m_nSomething; //0x0068
	char pad_006C[100]; //0x006C
}; //Size: 0x0158

class globalvars
{
public:
	float realtime;
	int framecount;
	float unfilteredframetime;
	float unkw;
	int maxClients;
	int tickcount;
	float interval_per_tick;
	float interval_per_tick_prediction;

};

struct Color
{
	byte r, g, b, a;
	float fR, fG, fB, fA;
};
class CSceneViewDebugOverlays
{
public:
	virtual void vfunc_0() = 0;
	virtual void vfunc_1() = 0;
	virtual void vfunc_2() = 0;
	virtual void vfunc_3() = 0;
	virtual void vfunc_4() = 0;
	virtual void vfunc_5() = 0;
	virtual void vfunc_6() = 0;
	virtual void vfunc_7() = 0;
	virtual void vfunc_8() = 0;
	virtual void vfunc_9() = 0;
	virtual void vfunc_10() = 0;
	virtual void vfunc_11() = 0;
	virtual void vfunc_12() = 0;
	virtual void DrawLine(Vector vEndPoint0, Vector vEndPoint1, Color color, bool bNoDepthTest, float flTimeToLive) = 0;
	virtual void Line2D(Vector2D& vStart, Vector2D& vEnd, Color color, float time) = 0;
	virtual void vfunc_15() = 0;
	virtual void vfunc_16() = 0;
	virtual void vfunc_17() = 0;
	virtual void vfunc_18() = 0;
	virtual void vfunc_19() = 0;
	virtual void vfunc_20() = 0;
	virtual void vfunc_21() = 0;
	virtual void VectorText3D(Vector vPos, Vector vUp, Vector vLeft, const char* pStr, Color vColor, bool bCenter, float flTimeToLive) = 0;
	virtual void DrawString(Vector vOrigin, int nTextLineOffsetY, const char* pText, float flMaxDistToDisplay, Color c, float flTimeToLive) = 0;
	virtual void DrawString2D(Vector2D& vOrigin, int nTextLineOffsetY, const char* pText, float flMaxDistToDisplay, Color c, float flTimeToLive) = 0;

};

struct hb_s0 {
	int64_t* f0;
	int64_t f8;
	int8_t pad512[496];
	int32_t f512;
	int8_t pad520[4];
	int16_t* f520;
	int8_t pad536[8];
	int32_t f536;
	int8_t pad544[4];
	int64_t f544;
	int8_t pad712[160];
	int32_t f712;
	int8_t pad720[4];
	int64_t f720;
};

struct  hb_s1 {
	int8_t pad96[96];
	uint16_t f96;
};


struct StringTable_t
{
	LDWORD vtable;
	LDWORD unkw;
	const char* pName;
};

