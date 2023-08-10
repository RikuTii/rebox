#include "main.h"
#include "sdk.h"
#include "Psapi.h"
#pragma comment( lib, "psapi.lib" )
#include "vector.h"
void* org_cmove = nullptr;
typedef unsigned long long LDWORD;
LDWORD client = NULL;
LDWORD engine = NULL;
LDWORD scenesystem = NULL;
LDWORD localplayer = NULL;
LDWORD entitylist = NULL;
LDWORD entitylist_addr = NULL;
LDWORD localplayer_addr = NULL;
LDWORD debugsystem_addr = NULL;
LDWORD screenmatrix_addr = NULL;
LDWORD globals_addr = NULL;
LDWORD source_client_addr = NULL;
LDWORD prediction_addr = NULL;
LDWORD stringtable_addr = NULL;


LDWORD current_localplayer = NULL;
std::string m_szPlayerClassname = "";

#define TIME_TO_TICKS(interval, dt )		( (int)( 0.5f + (float)(dt) / interval ) )
#define TICKS_TO_TIME(interval,  t )		( interval *( t ) )
#define RVA(Instr, InstrSize) ((uintptr_t)Instr + InstrSize + *(long*)((uintptr_t)Instr + (InstrSize - sizeof(long))))
bool Check(char* address, const std::string& signature)
{
	for (std::string::const_iterator it = signature.begin(); it != signature.end(); ++it)
	{
		if (*it == '?')
		{
			address++;
			continue;
		}

		if (*address != *it)
		{
			return false;
		}

		address++;
	}

	return true;
}
LDWORD FindPattern(HMODULE module, const std::string& signature)
{
	MODULEINFO info;

	if (!GetModuleInformation(GetCurrentProcess(), module, &info, sizeof(info)))
	{
		return 0;
	}


	char* end = ((char*)(module)+info.SizeOfImage);

	for (char* current = (char*)(module); current <= end; ++current)
	{
		if (Check(current, signature))
		{
			return (LDWORD)current;
		}
	}

	return 0;
}

void Log(const char* Message, ...)
{
	va_list list;
	char buffer[16384];
	va_start(list, Message);
	_vsnprintf_s(buffer, 16384, Message, list);
	va_end(list);

	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), 0, 0);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), "\n", strlen("\n"), 0, 0);
}

inline const PVOID* getvtable(const void* inst, size_t offset = 0)
{
	return *reinterpret_cast<const PVOID**>((size_t)inst + offset);
}
template< typename Fn >
inline Fn getvfunc(const void* inst, size_t index, size_t offset = 0)
{
	return reinterpret_cast<Fn>(getvtable(inst, offset)[index]);
}


#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))


void* CreateInterface(const char* ModuleName, const char* InterfaceName)
{
	typedef void* (__thiscall* CreateInterfaceFn)(const char*, void*);
	const auto Address = GetProcAddress(GetModuleHandleA(ModuleName), "CreateInterface");
	const auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(Address);

	return CreateInterface(InterfaceName, nullptr);
}

std::map<LDWORD, std::vector<LDWORD>> meshCache;
//Meshes used for entity hitboxes.
void CachePlayerMesh(LDWORD body)
{
	LDWORD skeleton = *(LDWORD*)(body + 0x8);
	if (skeleton)
	{
		LDWORD model_wrap = *(LDWORD*)(skeleton + 0x1A0);
		if (model_wrap)
		{
			LDWORD model = *(LDWORD*)(model_wrap);
			if (model)
			{
				LDWORD unkw = *(LDWORD*)(skeleton + 640);


				if (meshCache.find(body) != meshCache.end())
				{
					return;
				}
				else
				{
					std::vector<LDWORD> hitboxes;
					meshCache.insert({ body, hitboxes });
				}

				int meshCount = *(int*)(model + 112);
				if (meshCount)
				{
					LDWORD totalMeshCount = *(LDWORD*)(model + 144);
					for (int i = 0; i < meshCount; i++)
					{
						LDWORD something = *(LDWORD*)(totalMeshCount + (8 * i));
						if (unkw & something)
						{
							LDWORD nextMesh = NULL;
							if (i >= *(int*)(model + 112))
								nextMesh = 0;
							else
								nextMesh = *(LDWORD*)(*(LDWORD*)(model + 120) + 8 * i);

							if (nextMesh)
							{
								int meshDataPoints = *(int*)(nextMesh + 316);

								if (meshDataPoints)
								{
									int numMeshes = meshDataPoints;
									DWORD meshIter = 0x0;
									for (int j = meshDataPoints; j > 0; j--)
									{
										LDWORD nextInsideMesh = *(LDWORD*)(nextMesh + 296);

										int currentMesh = *(int*)(meshIter + nextInsideMesh + 40);
										if ((signed int)currentMesh > 0)
										{
											DWORD meshData = 0x0;
											for (int n = currentMesh; n > 0; n--)
											{
												LDWORD v25 = meshData + *(LDWORD*)(meshIter + *(LDWORD*)(nextMesh + 296) + 48);
												if (v25)
												{
													meshCache.at(body).push_back(v25);
												}
												meshData += 0x70;
											}
										}
										meshIter += 0x48;
									}
								}
							}

						}
					}
				}

			}
		}
	}
}


//snowman decompiler disassembly
int GetBoneMeshIndex(LDWORD model, LDWORD currentMesh)
{
	hb_s0* rcx = (hb_s0*)model;
	hb_s1* rdx = (hb_s1*)currentMesh;
	int index = *(int*)(currentMesh + 0x8);
	if (rcx->f512) {
		int rcx16 = index + index;
		uint16_t rdx17 = *reinterpret_cast<uint16_t*>(rcx->f720 + rcx16);
		*(int32_t*)((int64_t)&rdx17 + 4) = 0;
		int rcx18 = *reinterpret_cast<uint16_t*>(rcx->f544 + rcx16);
		*(int32_t*)((int64_t)&rcx18 + 4) = 0;
		uint32_t rax19 = rdx->f96;
		*(int32_t*)((int64_t)&rax19 + 4) = 0;
		int32_t rax20 = rcx->f520[rdx17 + rcx18 + rax19];
		*(int32_t*)((int64_t)&rax20 + 4) = 0;
		return  rax20;
	}

	return 3;
}

//Entity hitboxes used for hit registration
Vector GetHitboxPosition(LDWORD body, int hitbox)
{
	LDWORD skeleton = *(LDWORD*)(body + 0x8);
	if (skeleton)
	{
		LDWORD model_wrap = *(LDWORD*)(skeleton + 0x1A0);
		if (model_wrap)
		{
			LDWORD model = *(LDWORD*)(model_wrap);
			LDWORD skeletonCache = *(LDWORD*)(skeleton + 384);


			std::map<LDWORD, std::vector<LDWORD>>::iterator it = meshCache.find(body);
			if (it != meshCache.end())
			{
				int meshIndexIndex = hitbox;
				if (meshIndexIndex < it->second.size() && meshIndexIndex > 0)
				{
					LDWORD currentMesh = it->second.at(meshIndexIndex);
					int boneIndex = GetBoneMeshIndex(model, currentMesh);
					if (boneIndex > -1)
					{
						matrix3x4_t* transform = (matrix3x4_t*)(skeletonCache + (32 * boneIndex));
						matrix3x4_t* transform2 = (matrix3x4_t*)(skeletonCache + (32 * boneIndex + 16));//unscaled
						if (transform)
						{
							Vector hitbox_pos = Vector(transform2->m_flMatVal[1][0], transform2->m_flMatVal[1][1], transform2->m_flMatVal[1][2]);
							return hitbox_pos;
						}
					}
				}
			}
		}
	}

	return Vector(0, 0, 0);
}


//Check if point is visible or behind a wall.
bool IsTargetVisible(Vector& vStart, Vector& vEnd, LDWORD ignore)
{
	static LDWORD traceray = FindPattern(GetModuleHandleA("client.dll"), "\x40\x53\x48\x81\xEC????\x0F\x10\x02\x4C\x8D\x44\x24\x00\x48\x8B\xD9\x0F\x10\x4A\x10\x41\x0F\x11?\x0F\x10\x42\x20");

	TraceResult test_trace2;
	memset(&test_trace2, 0, sizeof(TraceResult));
	TraceRequest new_tr;
	memset(&new_tr, 0, sizeof(TraceRequest));

	new_tr.m_vecStart = vStart;
	new_tr.m_vecEnd = vEnd;
	new_tr.m_bEntities = false;
	new_tr.m_bWorld = true;
	new_tr.m_nMask = 0x10100000000;
	new_tr.m_pEnt = (void*)ignore;
	new_tr.m_nSomething = 1;
	typedef TraceResult* (__fastcall* do_a_trace)(TraceResult*, TraceRequest*);
	TraceResult* tr = ((do_a_trace)(traceray))(&test_trace2, &new_tr);

	return tr->m_Hitbox == 6 || tr->m_Hitbox == 28 || tr->m_Fraction > 0.92f;
}

//Player input handling
bool __fastcall hkCreateMove(UINT64* thisptr, int edx, void* arg1, void* arg2) {

	typedef bool(__thiscall* org)(void*, void*, void*);

	int seq_num = *(int*)((LDWORD)thisptr + 235232) + 1;

	if (seq_num < 1)
		return 0;


	CUserCmd* cmd = (CUserCmd*)((LDWORD)thisptr + 0x10 + (0x620 * (seq_num % 150)));
	if (!cmd)


	LDWORD localplayer = *(LDWORD*)(localplayer_addr);
	if (!localplayer || !cmd)
		return false;

	QAngle player_angles = *(QAngle*)(localplayer + 0x708);
	int buttons = *(int*)(localplayer + 0x6B0);
	Vector local_origin = *(Vector*)(localplayer + 0x740);
	int tick_base = *(int*)(localplayer + 0x6F0);

	LDWORD* entitylist = *(LDWORD**)(entitylist_addr);

	if (!entitylist)
		return 0;


	//handle current player inputs
	((org)(org_cmove))(thisptr, arg1, arg2);


	return false;
}



void* org_clientsimulate = nullptr;
//Called for each client frame
int __fastcall hkOnClientSimulate(void* thisptr, int edx) {
	typedef int(__thiscall* org)(void*);

	int retn = ((org)(org_clientsimulate))(thisptr);
	LDWORD* entitylist = *(LDWORD**)(entitylist_addr);


	LDWORD scene_debug = *(LDWORD*)(debugsystem_addr);

	int highestEnt = (unsigned int)(*(DWORD*)((LDWORD)entitylist + 8288) + 1);

	if (entitylist)
	{

		CSceneViewDebugOverlays* scene = (CSceneViewDebugOverlays*)((LDWORD)scene_debug);

		LDWORD localplayer = *(LDWORD*)(localplayer_addr);
		if (!localplayer)
			return retn;
		Vector local_origin = *(Vector*)(localplayer + 0x740);
		globalvars* globals = *(globalvars**)(globals_addr);

		if (globals)
		{
			Color red = {};
			red.r = 255;
			red.b = 0;
			red.a = 0;
			Vector2D Pos(100, 100);	

			scene->DrawString2D(Pos, 0, "hello world", 9999.0f, red, 1.0f);
		}



		static LDWORD local_sandboxplayer = NULL;

		if (current_localplayer == NULL && localplayer)
		{
			current_localplayer = localplayer;

			LDWORD prediction = (prediction_addr);

			int num_ents = *(int*)(prediction + 0x88);
			if (num_ents > 0)
			{
				DWORD iter = 0x0;
				if (num_ents > 0)
				{
					LDWORD v15 = *(LDWORD*)(0x8 + *(LDWORD*)(prediction + 144));
					if (v15)
					{
						const char* pName = *(const char**)(v15 + 0x3F8);
						std::string as_name = pName;
						size_t idx = as_name.find(' ');
						m_szPlayerClassname = as_name.substr(0, idx);
						Log("gamemode controlled entity is %s", m_szPlayerClassname.c_str());
						local_sandboxplayer = v15;

					}
				}
			}

		}

		if (current_localplayer != localplayer)
		{
			local_sandboxplayer = NULL;
			meshCache.clear();
			current_localplayer = NULL;
		}


		std::vector<LDWORD> player_bodies;
		for (int index = 0; index < highestEnt; index++)
		{
			DWORD offset = 0;
			if (index > 0)
			{
				offset = (((int)floor(index / 512)) * 0x8);
			}

			LDWORD entityCore = *(LDWORD*)((LDWORD)entitylist + 0x10 + offset);
			int currentIndex = index;
			if (offset > 0)
			{
				currentIndex = (index - ((offset / 8) * 512));
			}

			LDWORD tempentity = (LDWORD)(entityCore + (0x70 * currentIndex));

			if (tempentity)
			{

				LDWORD entity = *(LDWORD*)(tempentity);
				if (entity)
				{
					LDWORD body = *(LDWORD*)((LDWORD)entity + 0x38);
					if (body)
					{
						Vector origin = *(Vector*)(body + 0xD0);

						const char* pName = *(const char**)(entity + 0x3F8);

						std::string entityName = pName;

						if (entityName.find(m_szPlayerClassname) != std::string::npos) {

							player_bodies.push_back(body);
							int flags = *(int*)(entity + 0x5A4);
							bool is_alive = (flags == 31);

							if (!is_alive)
								continue;

							if (!local_sandboxplayer)
								continue;

							if (entity == local_sandboxplayer)
								continue;
							LDWORD skeleton = *(LDWORD*)(body + 0x8);
							if (skeleton && is_alive)
							{
								LDWORD model_wrap = *(LDWORD*)(skeleton + 0x1A0);
								if (model_wrap)
								{

									LDWORD model = *(LDWORD*)(model_wrap);
									if (model)
									{
										CachePlayerMesh(body);
									}
								}
							}
						}
					}
				}
			}
		}


		for (auto it = meshCache.cbegin(); it != meshCache.cend();)
		{
			if (std::find(player_bodies.begin(), player_bodies.end(), it->first) != std::end(player_bodies))
			{
				++it;
			}
			else
			{
				meshCache.erase(it++);
			}
		}
	}

	return retn;
}




void* org_scenesystem_drawmodel = nullptr;
//Drawing all entity models
int __cdecl hkDrawModel(void* thisptr, void* edx, __int64 a3, unsigned int a4, __int64 a5)
{
	typedef int(__cdecl* org)(void*, void*, __int64, unsigned int, __int64);
	LDWORD mesh_inst = *(LDWORD*)((LDWORD)edx + 8);

	if (mesh_inst)
	{
		LDWORD mesh_data = *(LDWORD*)(mesh_inst + 8);
		if (mesh_data)
		{
			LDWORD model = *(LDWORD*)(mesh_data);
			const char* cmodel_name = *(const char**)(model + 0x8);


			std::string model_name = cmodel_name;
			if (model_name.find("citizen") != std::string::npos)
			{
				BYTE r, g, b;
				float a;
				r = *(byte*)((LDWORD)edx + 0x5C);
				g = *(byte*)((LDWORD)edx + 0x5D);
				b = *(byte*)((LDWORD)edx + 0x5E);
				a = *(float*)((LDWORD)edx + 0x64);


				*(byte*)((LDWORD)edx + 0x5C) = 255;
				*(byte*)((LDWORD)edx + 0x5D) = 0;
				*(byte*)((LDWORD)edx + 0x5E) = 0;
				*(float*)((LDWORD)edx + 0x64) = 1.0f;

				//just game material pointers
				LDWORD material = *(LDWORD*)(scenesystem + 0x1D3360);
				LDWORD material_ignorez = *(LDWORD*)(scenesystem + 0x1D3368);

				int retn = 0;

				//swap material reference
				for (int i = 0; i < 5; i++)
				{
					LDWORD meshInstance = *(LDWORD*)a5 + 0x78 * i;
					LDWORD pMaterial2 = *(LDWORD*)material_ignorez;
					*(LDWORD*)(meshInstance + 0x18) = *(LDWORD*)material_ignorez;
					*(LDWORD*)(meshInstance + 0x20) = pMaterial2;
				}

				//draw model again using custom colors and material
				retn = ((org)(org_scenesystem_drawmodel))(thisptr, edx, a3, a4, a5);

				for (int i = 0; i < 5; i++)
				{
					LDWORD meshInstance = *(LDWORD*)a5 + 0x78 * i;
					LDWORD pMaterial = *(LDWORD*)material;

					*(LDWORD*)(meshInstance + 0x18) = *(LDWORD*)material;
					*(LDWORD*)(meshInstance + 0x20) = pMaterial;
				}

				if (retn)
				{
					*(byte*)((LDWORD)edx + 0x5C) = 0;
					*(byte*)((LDWORD)edx + 0x5D) = 255;
					*(byte*)((LDWORD)edx + 0x5E) = 0;
					*(float*)((LDWORD)edx + 0x64) = (0.1f * 9);

					//draw model with new colors and reduced alpha
					retn = ((org)(org_scenesystem_drawmodel))(thisptr, edx, a3, a4, a5);
				}


				*(byte*)((LDWORD)edx + 0x5C) = r;
				*(byte*)((LDWORD)edx + 0x5D) = g;
				*(byte*)((LDWORD)edx + 0x5E) = b;

				return retn;


			}

			//reduce map alpha modulation to allow pass through rendering
			if (model_name.find("maps") != std::string::npos)
			{
				*(float*)((LDWORD)edx + 0x64) = 0.99999f;
				int uw = ((org)(org_scenesystem_drawmodel))(thisptr, edx, a3, a4, a5);
				return uw;
			}
		}
	}


	return ((org)(org_scenesystem_drawmodel))(thisptr, edx, a3, a4, a5);
}



void SetupInterfaces()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());

	Log("Start injection");

	if (MH_Initialize() != MH_OK) {
		Log("failed init");
	}

	source_client_addr = (LDWORD)CreateInterface("client.dll", "Source2Client002");
	engine = (std::int64_t)GetModuleHandleA("engine2.dll");
	client = (std::int64_t)GetModuleHandleA("client.dll");
	scenesystem = (std::int64_t)GetModuleHandleA("scenesystem.dll");

	localplayer_addr = RVA(FindPattern(GetModuleHandleA("client.dll"), "\x48\x8B\x35????\x48\x8D\x85????\x48\x89\x85????\x48\x8D\x05????\x48\x89\x85"), 7);
	entitylist_addr = RVA(FindPattern(GetModuleHandleA("client.dll"), "\x48\x8B\x05????\x48\x8B\x0E\x48\x8B\x98????\x48\x8B\x01\xFF\x10\x4C\x8B\x13\x48\x8D\x4C\x24?"), 7);
	debugsystem_addr = RVA(FindPattern(GetModuleHandleA("client.dll"), "\x48\x8B\x0D????\x4C\x8D\x45\xA8\xF3\x44\x0F\x59\xC6\x48\x8D\x54\x24?\xC7\x85"), 7);
	screenmatrix_addr = RVA(FindPattern(GetModuleHandleA("client.dll"), "\x4C\x8D\x05????\x48\x8D\x15????\x48\x8D\x0D????\xE8????\x66\x0F\x6E\x87????"), 7);

	globals_addr = RVA(FindPattern(GetModuleHandleA("client.dll"), "\x48\x89\x15????\x48\x8D\x05????\x48\x85\xD2\x75\x23"), 7);
	prediction_addr = RVA(FindPattern(GetModuleHandleA("client.dll"), "\x48\x8B\x05????\x48\x8D\x0D????\xFF\x90????\x0F\x28\xCF\x33\xC9\xE8????"), 7);
	stringtable_addr = RVA(FindPattern(GetModuleHandleA("client.dll"), "\x48\x8B\x0D????\x48\x8D\x15????\x48\x8B\x01\xFF\x50\x70\x4C\x8B\xF8\x48\x85\xC0\x0F\x84????"), 7);



	std::int64_t createmove_addr = FindPattern(GetModuleHandleA("client.dll"), "\x48\x89\x5C\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xEC?\xFF\x81");;
	Log("client %llx %llx", createmove_addr, client);

	if (MH_CreateHook((LPVOID)createmove_addr, hkCreateMove,
		reinterpret_cast<LPVOID*>(&org_cmove)) != MH_OK) {
		Log("failed hook");
	}

	LDWORD client_simulate = FindPattern(GetModuleHandleA("client.dll"), "\x48\x83\xEC\x28\xE8????\x48\x83\xC4\x28\x48\xFF\x25????");

	if (MH_CreateHook((LPVOID)client_simulate, hkOnClientSimulate,
		reinterpret_cast<LPVOID*>(&org_clientsimulate)) != MH_OK) {
		Log("failed hook simulate");
	}



	MH_EnableHook((LPVOID)createmove_addr);
	MH_EnableHook((LPVOID)client_simulate);
	MH_EnableHook((LPVOID)client_simulate);


	Log("Injected hook");

}
