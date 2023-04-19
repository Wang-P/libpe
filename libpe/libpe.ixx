module;
/**********************************************************************
* Copyright © 2018-2023, Jovibor: https://github.com/jovibor/         *
* Library for parsing PE32 (x86) and PE32+ (x64) binaries.            *
* Official git repository: https://github.com/jovibor/libpe           *
* This software is available under the "MIT License".                 *
**********************************************************************/
#include <Windows.h>
#include <WinTrust.h> //WIN_CERTIFICATE struct.
#include <cassert>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <strsafe.h>
#include <unordered_map>
#include <vector>

export module libpe;

namespace libpe
{
	export constexpr auto LIBPE_VERSION_MAJOR = 2;
	export constexpr auto LIBPE_VERSION_MINOR = 0;
	export constexpr auto LIBPE_VERSION_PATCH = 0;

	//Rich.
	export struct PERICHHDR {
		DWORD dwOffset; //File's raw offset of this entry.
		WORD  wId;      //Entry Id.
		WORD  wVersion; //Entry version.
		DWORD dwCount;  //Amount of occurrences.
	};
	export using PERICHHDR_VEC = std::vector<PERICHHDR>;

	//NT header.
	export struct PENTHDR {
		DWORD dwOffset;   //File's raw offset of this header.
		union UNPENTHDR { //Union of either x86 or x64 NT header.
			IMAGE_NT_HEADERS32 stNTHdr32; //x86 Header.
			IMAGE_NT_HEADERS64 stNTHdr64; //x64 Header.
		} unHdr;
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapFileHdrMachine {
		{ IMAGE_FILE_MACHINE_UNKNOWN, L"IMAGE_FILE_MACHINE_UNKNOWN" },
		{ IMAGE_FILE_MACHINE_TARGET_HOST, L"IMAGE_FILE_MACHINE_TARGET_HOST" },
		{ IMAGE_FILE_MACHINE_I386, L"IMAGE_FILE_MACHINE_I386" },
		{ IMAGE_FILE_MACHINE_R3000, L"IMAGE_FILE_MACHINE_R3000" },
		{ IMAGE_FILE_MACHINE_R4000, L"IMAGE_FILE_MACHINE_R4000" },
		{ IMAGE_FILE_MACHINE_R10000, L"IMAGE_FILE_MACHINE_R10000" },
		{ IMAGE_FILE_MACHINE_WCEMIPSV2, L"IMAGE_FILE_MACHINE_WCEMIPSV2" },
		{ IMAGE_FILE_MACHINE_ALPHA, L"IMAGE_FILE_MACHINE_ALPHA" },
		{ IMAGE_FILE_MACHINE_SH3, L"IMAGE_FILE_MACHINE_SH3" },
		{ IMAGE_FILE_MACHINE_SH3DSP, L"IMAGE_FILE_MACHINE_SH3DSP" },
		{ IMAGE_FILE_MACHINE_SH3E, L"IMAGE_FILE_MACHINE_SH3E" },
		{ IMAGE_FILE_MACHINE_SH4, L"IMAGE_FILE_MACHINE_SH4" },
		{ IMAGE_FILE_MACHINE_SH5, L"IMAGE_FILE_MACHINE_SH5" },
		{ IMAGE_FILE_MACHINE_ARM, L"IMAGE_FILE_MACHINE_ARM" },
		{ IMAGE_FILE_MACHINE_THUMB, L"IMAGE_FILE_MACHINE_THUMB" },
		{ IMAGE_FILE_MACHINE_ARMNT, L"IMAGE_FILE_MACHINE_ARMNT" },
		{ IMAGE_FILE_MACHINE_AM33, L"IMAGE_FILE_MACHINE_AM33" },
		{ IMAGE_FILE_MACHINE_POWERPC, L"IMAGE_FILE_MACHINE_POWERPC" },
		{ IMAGE_FILE_MACHINE_POWERPCFP, L"IMAGE_FILE_MACHINE_POWERPCFP" },
		{ IMAGE_FILE_MACHINE_IA64, L"IMAGE_FILE_MACHINE_IA64" },
		{ IMAGE_FILE_MACHINE_MIPS16, L"IMAGE_FILE_MACHINE_MIPS16" },
		{ IMAGE_FILE_MACHINE_ALPHA64, L"IMAGE_FILE_MACHINE_ALPHA64" },
		{ IMAGE_FILE_MACHINE_MIPSFPU, L"IMAGE_FILE_MACHINE_MIPSFPU" },
		{ IMAGE_FILE_MACHINE_MIPSFPU16, L"IMAGE_FILE_MACHINE_MIPSFPU16" },
		{ IMAGE_FILE_MACHINE_TRICORE, L"IMAGE_FILE_MACHINE_TRICORE" },
		{ IMAGE_FILE_MACHINE_CEF, L"IMAGE_FILE_MACHINE_CEF" },
		{ IMAGE_FILE_MACHINE_EBC, L"IMAGE_FILE_MACHINE_EBC" },
		{ IMAGE_FILE_MACHINE_AMD64, L"IMAGE_FILE_MACHINE_AMD64" },
		{ IMAGE_FILE_MACHINE_M32R, L"IMAGE_FILE_MACHINE_M32R" },
		{ IMAGE_FILE_MACHINE_ARM64, L"IMAGE_FILE_MACHINE_ARM64" },
		{ IMAGE_FILE_MACHINE_CEE, L"IMAGE_FILE_MACHINE_CEE" },
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapFileHdrCharact {
		{ IMAGE_FILE_RELOCS_STRIPPED, L"IMAGE_FILE_RELOCS_STRIPPED" },
		{ IMAGE_FILE_EXECUTABLE_IMAGE, L"IMAGE_FILE_EXECUTABLE_IMAGE" },
		{ IMAGE_FILE_LINE_NUMS_STRIPPED, L"IMAGE_FILE_LINE_NUMS_STRIPPED" },
		{ IMAGE_FILE_LOCAL_SYMS_STRIPPED, L"IMAGE_FILE_LOCAL_SYMS_STRIPPED" },
		{ IMAGE_FILE_AGGRESIVE_WS_TRIM, L"IMAGE_FILE_AGGRESIVE_WS_TRIM" },
		{ IMAGE_FILE_LARGE_ADDRESS_AWARE, L"IMAGE_FILE_LARGE_ADDRESS_AWARE" },
		{ IMAGE_FILE_BYTES_REVERSED_LO, L"IMAGE_FILE_BYTES_REVERSED_LO" },
		{ IMAGE_FILE_32BIT_MACHINE, L"IMAGE_FILE_32BIT_MACHINE" },
		{ IMAGE_FILE_DEBUG_STRIPPED, L"IMAGE_FILE_DEBUG_STRIPPED" },
		{ IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP, L"IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP" },
		{ IMAGE_FILE_NET_RUN_FROM_SWAP, L"IMAGE_FILE_NET_RUN_FROM_SWAP" },
		{ IMAGE_FILE_SYSTEM, L"IMAGE_FILE_SYSTEM" },
		{ IMAGE_FILE_DLL, L"IMAGE_FILE_DLL" },
		{ IMAGE_FILE_UP_SYSTEM_ONLY, L"IMAGE_FILE_UP_SYSTEM_ONLY" },
		{ IMAGE_FILE_BYTES_REVERSED_HI, L"IMAGE_FILE_BYTES_REVERSED_HI" }
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapOptHdrMagic {
		{ IMAGE_NT_OPTIONAL_HDR32_MAGIC, L"IMAGE_NT_OPTIONAL_HDR32_MAGIC" },
		{ IMAGE_NT_OPTIONAL_HDR64_MAGIC, L"IMAGE_NT_OPTIONAL_HDR64_MAGIC" },
		{ IMAGE_ROM_OPTIONAL_HDR_MAGIC, L"IMAGE_ROM_OPTIONAL_HDR_MAGIC" }
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapOptHdrSubsystem {
		{ IMAGE_SUBSYSTEM_UNKNOWN, L"IMAGE_SUBSYSTEM_UNKNOWN" },
		{ IMAGE_SUBSYSTEM_NATIVE, L"IMAGE_SUBSYSTEM_NATIVE" },
		{ IMAGE_SUBSYSTEM_WINDOWS_GUI, L"IMAGE_SUBSYSTEM_WINDOWS_GUI" },
		{ IMAGE_SUBSYSTEM_WINDOWS_CUI, L"IMAGE_SUBSYSTEM_WINDOWS_CUI" },
		{ IMAGE_SUBSYSTEM_OS2_CUI, L"IMAGE_SUBSYSTEM_OS2_CUI" },
		{ IMAGE_SUBSYSTEM_POSIX_CUI, L"IMAGE_SUBSYSTEM_POSIX_CUI" },
		{ IMAGE_SUBSYSTEM_NATIVE_WINDOWS, L"IMAGE_SUBSYSTEM_NATIVE_WINDOWS" },
		{ IMAGE_SUBSYSTEM_WINDOWS_CE_GUI, L"IMAGE_SUBSYSTEM_WINDOWS_CE_GUI" },
		{ IMAGE_SUBSYSTEM_EFI_APPLICATION, L"IMAGE_SUBSYSTEM_EFI_APPLICATION" },
		{ IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER, L"IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER" },
		{ IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER, L"IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER" },
		{ IMAGE_SUBSYSTEM_EFI_ROM, L"IMAGE_SUBSYSTEM_EFI_ROM" },
		{ IMAGE_SUBSYSTEM_XBOX, L"IMAGE_SUBSYSTEM_XBOX" },
		{ IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION, L"IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION" },
		{ IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG, L"IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG" }
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapOptHdrDllCharact {
		{ 0x0001, L"IMAGE_LIBRARY_PROCESS_INIT" },
		{ 0x0002, L"IMAGE_LIBRARY_PROCESS_TERM" },
		{ 0x0004, L"IMAGE_LIBRARY_THREAD_INIT" },
		{ 0x0008, L"IMAGE_LIBRARY_THREAD_TERM" },
		{ IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA, L"IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA" },
		{ IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE, L"IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE" },
		{ IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY, L"IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY" },
		{ IMAGE_DLLCHARACTERISTICS_NX_COMPAT, L"IMAGE_DLLCHARACTERISTICS_NX_COMPAT" },
		{ IMAGE_DLLCHARACTERISTICS_NO_ISOLATION, L"IMAGE_DLLCHARACTERISTICS_NO_ISOLATION" },
		{ IMAGE_DLLCHARACTERISTICS_NO_SEH, L"IMAGE_DLLCHARACTERISTICS_NO_SEH" },
		{ IMAGE_DLLCHARACTERISTICS_NO_BIND, L"IMAGE_DLLCHARACTERISTICS_NO_BIND" },
		{ IMAGE_DLLCHARACTERISTICS_APPCONTAINER, L"IMAGE_DLLCHARACTERISTICS_APPCONTAINER" },
		{ IMAGE_DLLCHARACTERISTICS_WDM_DRIVER, L"IMAGE_DLLCHARACTERISTICS_WDM_DRIVER" },
		{ IMAGE_DLLCHARACTERISTICS_GUARD_CF, L"IMAGE_DLLCHARACTERISTICS_GUARD_CF" },
		{ IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE, L"IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE" }
	};

	//Data directories.
	export struct PEDATADIR {
		IMAGE_DATA_DIRECTORY stDataDir;  //Standard header.
		std::string          strSection; //Name of the section this directory resides in (points to).
	};
	export using PEDATADIR_VEC = std::vector<PEDATADIR>;

	//Sections headers.
	//For more info check:
	//docs.microsoft.com/en-us/windows/desktop/api/winnt/ns-winnt-_image_section_header#members
	//«An 8-byte, null-padded UTF-8 string. For longer names, this member contains a forward slash (/) 
	//followed by an ASCII representation of a decimal number that is an offset into the string table.»
	export struct PESECHDR {
		DWORD                dwOffset;   //File's raw offset of this section header descriptor.
		IMAGE_SECTION_HEADER stSecHdr;   //Standard section header.
		std::string          strSecName; //Section full name.
	};
	export using PESECHDR_VEC = std::vector<PESECHDR>;

	export inline const std::unordered_map<DWORD, std::wstring_view> MapSecHdrCharact {
		{ 0x00000000, L"IMAGE_SCN_TYPE_REG (Reserved)" },
		{ 0x00000001, L"IMAGE_SCN_TYPE_DSECT (Reserved)" },
		{ 0x00000002, L"IMAGE_SCN_TYPE_NOLOAD (Reserved)" },
		{ 0x00000004, L"IMAGE_SCN_TYPE_GROUP (Reserved)" },
		{ IMAGE_SCN_TYPE_NO_PAD, L"IMAGE_SCN_TYPE_NO_PAD (Reserved)" },
		{ 0x00000010, L"IMAGE_SCN_TYPE_COPY (Reserved)" },
		{ IMAGE_SCN_CNT_CODE, L"IMAGE_SCN_CNT_CODE (Section contains code)" },
		{ IMAGE_SCN_CNT_INITIALIZED_DATA, L"IMAGE_SCN_CNT_INITIALIZED_DATA (Section contains initialized data)" },
		{ IMAGE_SCN_CNT_UNINITIALIZED_DATA, L"IMAGE_SCN_CNT_UNINITIALIZED_DATA (Section contains uninitialized data)" },
		{ IMAGE_SCN_LNK_OTHER, L"IMAGE_SCN_LNK_OTHER (Reserved)" },
		{ IMAGE_SCN_LNK_INFO, L"IMAGE_SCN_LNK_INFO (Section contains comments or some other type of information)" },
		{ 0x00000400, L"IMAGE_SCN_TYPE_OVER (Reserved)" },
		{ IMAGE_SCN_LNK_REMOVE, L"IMAGE_SCN_LNK_REMOVE (Section contents will not become part of image)" },
		{ IMAGE_SCN_LNK_COMDAT, L"IMAGE_SCN_LNK_COMDAT (Section contents comdat)" },
		{ IMAGE_SCN_NO_DEFER_SPEC_EXC, L"IMAGE_SCN_NO_DEFER_SPEC_EXC (Reset speculative exceptions handling bits in the TLB entries for this section)" },
		{ IMAGE_SCN_GPREL, L"IMAGE_SCN_GPREL (Section content can be accessed relative to GP)" },
		{ 0x00010000, L"IMAGE_SCN_MEM_SYSHEAP (Obsolete)" },
		{ IMAGE_SCN_MEM_PURGEABLE, L"IMAGE_SCN_MEM_PURGEABLE" },
		{ IMAGE_SCN_MEM_LOCKED, L"IMAGE_SCN_MEM_LOCKED" },
		{ IMAGE_SCN_MEM_PRELOAD, L"IMAGE_SCN_MEM_PRELOAD" },
		{ IMAGE_SCN_ALIGN_1BYTES, L"IMAGE_SCN_ALIGN_1BYTES" },
		{ IMAGE_SCN_ALIGN_2BYTES, L"IMAGE_SCN_ALIGN_2BYTES" },
		{ IMAGE_SCN_ALIGN_4BYTES, L"IMAGE_SCN_ALIGN_4BYTES" },
		{ IMAGE_SCN_ALIGN_8BYTES, L"IMAGE_SCN_ALIGN_8BYTES" },
		{ IMAGE_SCN_ALIGN_16BYTES, L"IMAGE_SCN_ALIGN_16BYTES (Default alignment if no others are specified)" },
		{ IMAGE_SCN_ALIGN_32BYTES, L"IMAGE_SCN_ALIGN_32BYTES" },
		{ IMAGE_SCN_ALIGN_64BYTES, L"IMAGE_SCN_ALIGN_64BYTES" },
		{ IMAGE_SCN_ALIGN_128BYTES, L"IMAGE_SCN_ALIGN_128BYTES" },
		{ IMAGE_SCN_ALIGN_256BYTES, L"IMAGE_SCN_ALIGN_256BYTES" },
		{ IMAGE_SCN_ALIGN_512BYTES, L"IMAGE_SCN_ALIGN_512BYTES" },
		{ IMAGE_SCN_ALIGN_1024BYTES, L"IMAGE_SCN_ALIGN_1024BYTES" },
		{ IMAGE_SCN_ALIGN_2048BYTES, L"IMAGE_SCN_ALIGN_2048BYTES" },
		{ IMAGE_SCN_ALIGN_4096BYTES, L"IMAGE_SCN_ALIGN_4096BYTES" },
		{ IMAGE_SCN_ALIGN_8192BYTES, L"IMAGE_SCN_ALIGN_8192BYTES" },
		{ IMAGE_SCN_ALIGN_MASK, L"IMAGE_SCN_ALIGN_MASK" },
		{ IMAGE_SCN_LNK_NRELOC_OVFL, L"IMAGE_SCN_LNK_NRELOC_OVFL (Section contains extended relocations)" },
		{ IMAGE_SCN_MEM_DISCARDABLE, L"IMAGE_SCN_MEM_DISCARDABLE (Section can be discarded)" },
		{ IMAGE_SCN_MEM_NOT_CACHED, L"IMAGE_SCN_MEM_NOT_CACHED (Section is not cachable)" },
		{ IMAGE_SCN_MEM_NOT_PAGED, L"IMAGE_SCN_MEM_NOT_PAGED (Section is not pageable)" },
		{ IMAGE_SCN_MEM_SHARED, L"IMAGE_SCN_MEM_SHARED (Section is shareable)" },
		{ IMAGE_SCN_MEM_EXECUTE, L"IMAGE_SCN_MEM_EXECUTE (Section is executable)" },
		{ IMAGE_SCN_MEM_READ, L"IMAGE_SCN_MEM_READ (Section is readable)" },
		{ IMAGE_SCN_MEM_WRITE, L"IMAGE_SCN_MEM_WRITE (Section is writeable)" }
	};

	//Export table.
	export struct PEEXPORTFUNC {
		DWORD       dwFuncRVA;        //Function RVA.
		DWORD       dwOrdinal;        //Function ordinal.
		DWORD       dwNameRVA;        //Name RVA.
		std::string strFuncName;      //Function name.
		std::string strForwarderName; //Function forwarder name.
	};
	export struct PEEXPORT {
		DWORD                     dwOffset;      //File's raw offset of the Export header descriptor.
		IMAGE_EXPORT_DIRECTORY    stExportDesc;  //Standard export header descriptor.
		std::string               strModuleName; //Actual module name.
		std::vector<PEEXPORTFUNC> vecFuncs;      //Array of the exported functions struct.	
	};

	//Import table:
	export struct PEIMPORTFUNC {
		union UNPEIMPORTTHUNK {
			IMAGE_THUNK_DATA32 stThunk32; //x86 standard thunk.
			IMAGE_THUNK_DATA64 stThunk64; //x64 standard thunk.
		} unThunk;
		IMAGE_IMPORT_BY_NAME stImpByName; //Standard IMAGE_IMPORT_BY_NAME struct
		std::string          strFuncName; //Function name.
	};
	export struct PEIMPORT {
		DWORD                     dwOffset;      //File's raw offset of this Import descriptor.
		IMAGE_IMPORT_DESCRIPTOR   stImportDesc;  //Standard Import descriptor.
		std::string               strModuleName; //Imported module name.
		std::vector<PEIMPORTFUNC> vecImportFunc; //Array of imported functions.
	};
	export using PEIMPORT_VEC = std::vector<PEIMPORT>;

	/**************************************Resources by Levels*******************************************
	* There are 3 levels of resources: 1. Type 2. Name 3. Language.										*
	* https://docs.microsoft.com/en-us/windows/desktop/Debug/pe-format#the-rsrc-section					*
	* Highest (root) resource structure is PERESROOT. It's a struct	that includes:                      *
	* an IMAGE_RESOURCE_DIRECTORY of root resource directory itself and vector<PERESROOTDATA>,          *
	* that contains structs of all IMAGE_RESOURCE_DIRECTORY_ENTRY structures of the root resource       *
	* directory. It also includes: wstring(Resource name), IMAGE_RESOURCE_DATA_ENTRY,                   *
	* vector<byte> (RAW resource data), and PERESLVL2 that is a struct of the next, second, resource    *
	* level, that replicates struct of root resource level.	PERESLVL2 includes IMAGE_RESOURCE_DIRECTORY *
	* of the second resource level, and vector<PERESLVL2DATA> that includes PERESLVL3 that is a struct  *
	* of the last, third, level of resources. Like previous two, this last level's struct consist of    *
	* IMAGE_RESOURCE_DIRECTORY and vector<PERESLVL3DATA>, that is again a vector of structs of all      *
	* IMAGE_RESOURCE_DIRECTORY_ENTRY of the last, third, level of resources.                            *
	****************************************************************************************************/

	//Level 3/Lang (the lowest) resources.
	export struct PERESLVL3DATA {
		IMAGE_RESOURCE_DIRECTORY_ENTRY stResDirEntry;  //Level 3 (Lang) standard IMAGE_RESOURCE_DIRECTORY_ENTRY struct.
		std::wstring                   wstrResName;    //Level 3 (Lang) resource name.
		IMAGE_RESOURCE_DATA_ENTRY      stResDataEntry; //Level 3 (Lang) standard IMAGE_RESOURCE_DATA_ENTRY struct.
		std::vector<std::byte>         vecRawResData;  //Level 3 (Lang) resource raw data.
	};
	export using PERESLANGDATA = PERESLVL3DATA;

	export struct PERESLVL3 {
		DWORD                      dwOffset;   //File's raw offset of this level 3 IMAGE_RESOURCE_DIRECTORY descriptor.
		IMAGE_RESOURCE_DIRECTORY   stResDir;   //Level 3 standard IMAGE_RESOURCE_DIRECTORY header.
		std::vector<PERESLVL3DATA> vecResData; //Array of level 3 resource entries.
	};
	export using PERESLANG = PERESLVL3;

	//Level 2/Name resources — Includes Lang resourses.
	export struct PERESLVL2DATA {
		IMAGE_RESOURCE_DIRECTORY_ENTRY stResDirEntry;  //Level 2 (Name) standard IMAGE_RESOURCE_DIRECTORY_ENTRY struct.
		std::wstring                   wstrResName;    //Level 2 (Name) resource name.
		IMAGE_RESOURCE_DATA_ENTRY      stResDataEntry; //Level 2 (Name) standard IMAGE_RESOURCE_DATA_ENTRY struct.
		std::vector<std::byte>         vecRawResData;  //Level 2 (Name) resource raw data.
		PERESLVL3                      stResLvL3;      //Level 3 (Lang) resource struct.
	};
	export using PERESNAMEDATA = PERESLVL2DATA;

	export struct PERESLVL2 {
		DWORD                      dwOffset;   //File's raw offset of this level 2 IMAGE_RESOURCE_DIRECTORY descriptor.
		IMAGE_RESOURCE_DIRECTORY   stResDir;   //Level 2 standard IMAGE_RESOURCE_DIRECTORY header.
		std::vector<PERESLVL2DATA> vecResData; //Array of level 2 resource entries.
	};
	export using PERESNAME = PERESLVL2;

	//Level 1/Type resources — Includes Name Resources.
	export struct PERESROOTDATA {
		IMAGE_RESOURCE_DIRECTORY_ENTRY stResDirEntry;  //Level root (Type) standard IMAGE_RESOURCE_DIRECTORY_ENTRY struct.
		std::wstring                   wstrResName;	   //Level root (Type) resource name.
		IMAGE_RESOURCE_DATA_ENTRY      stResDataEntry; //Level root (Type) standard IMAGE_RESOURCE_DATA_ENTRY struct.
		std::vector<std::byte>         vecRawResData;  //Level root (Type) resource raw data.
		PERESLVL2                      stResLvL2;      //Level 2 (Name) resource struct.
	};
	export using PERESTYPEDATA = PERESROOTDATA;

	export struct PERESROOT {
		DWORD                      dwOffset;   //File's raw offset of this level root IMAGE_RESOURCE_DIRECTORY descriptor.
		IMAGE_RESOURCE_DIRECTORY   stResDir;   //Level root standard IMAGE_RESOURCE_DIRECTORY header.
		std::vector<PERESROOTDATA> vecResData; //Array of level root resource entries.
	};
	export using PERESTYPE = PERESROOT;

	//Flattened resources.
	export struct PERESFLAT {
		std::span<const std::byte> spnData { };    //Resource data.
		std::wstring_view          wsvTypeStr { }; //Resource Type name.
		std::wstring_view          wsvNameStr { }; //Resource Name name (resource itself name).
		std::wstring_view          wsvLangStr { }; //Resource Lang name.
		WORD                       wTypeID { };    //Resource Type ID (RT_CURSOR, RT_BITMAP, etc...).
		WORD                       wNameID { };    //Resource Name ID (resource itself ID).
		WORD                       wLangID { };    //Resource Lang ID.
	};
	export using PERESFLAT_VEC = std::vector<PERESFLAT>;
	export inline const std::unordered_map<DWORD, std::wstring_view> MapResID {
		{ 1, L"RT_CURSOR" },
		{ 2, L"RT_BITMAP" },
		{ 3, L"RT_ICON" },
		{ 4, L"RT_MENU" },
		{ 5, L"RT_DIALOG" },
		{ 6, L"RT_STRING" },
		{ 7, L"RT_FONTDIR" },
		{ 8, L"RT_FONT" },
		{ 9, L"RT_ACCELERATOR" },
		{ 10, L"RT_RCDATA" },
		{ 11, L"RT_MESSAGETABLE" },
		{ 12, L"RT_GROUP_CURSOR" },
		{ 14, L"RT_GROUP_ICON" },
		{ 16, L"RT_VERSION" },
		{ 17, L"RT_DLGINCLUDE" },
		{ 19, L"RT_PLUGPLAY" },
		{ 20, L"RT_VXD" },
		{ 21, L"RT_ANICURSOR" },
		{ 22, L"RT_ANIICON" },
		{ 23, L"RT_HTML" },
		{ 24, L"RT_MANIFEST" },
		{ 28, L"RT_RIBBON_XML" },
		{ 240, L"RT_DLGINIT" },
		{ 241, L"RT_TOOLBAR" }
	};
	/*********************************Resources End*****************************************/

	//Exception table.
	export struct PEEXCEPTION {
		DWORD                         dwOffset;           //File's raw offset of this exception's descriptor.
		_IMAGE_RUNTIME_FUNCTION_ENTRY stRuntimeFuncEntry; //Standard _IMAGE_RUNTIME_FUNCTION_ENTRY header.
	};
	export using PEEXCEPTION_VEC = std::vector<PEEXCEPTION>;

	//Security table.
	export struct PESECURITY {
		DWORD           dwOffset;  //File's raw offset of this security descriptor.
		WIN_CERTIFICATE stWinSert; //Standard WIN_CERTIFICATE header.
	};
	export using PESECURITY_VEC = std::vector<PESECURITY>;
	export inline const std::unordered_map<DWORD, std::wstring_view> MapWinCertRevision {
		{ WIN_CERT_REVISION_1_0, L"WIN_CERT_REVISION_1_0" },
		{ WIN_CERT_REVISION_2_0, L"WIN_CERT_REVISION_2_0" }
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapWinCertType {
		{ WIN_CERT_TYPE_X509, L"WIN_CERT_TYPE_X509" },
		{ WIN_CERT_TYPE_PKCS_SIGNED_DATA, L"WIN_CERT_TYPE_PKCS_SIGNED_DATA" },
		{ WIN_CERT_TYPE_RESERVED_1, L"WIN_CERT_TYPE_RESERVED_1" },
		{ WIN_CERT_TYPE_TS_STACK_SIGNED, L"WIN_CERT_TYPE_TS_STACK_SIGNED" },
	};

	//Relocation table.
	export struct PERELOCDATA {
		DWORD dwOffset;     //File's raw offset of this Relocation data descriptor.
		WORD  wRelocType;   //Relocation type.
		WORD  wRelocOffset; //Relocation offset (Offset the relocation must be applied to.)
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapRelocType {
		{ IMAGE_REL_BASED_ABSOLUTE, L"IMAGE_REL_BASED_ABSOLUTE" },
		{ IMAGE_REL_BASED_HIGH, L"IMAGE_REL_BASED_HIGH" },
		{ IMAGE_REL_BASED_LOW, L"IMAGE_REL_BASED_LOW" },
		{ IMAGE_REL_BASED_HIGHLOW, L"IMAGE_REL_BASED_HIGHLOW" },
		{ IMAGE_REL_BASED_HIGHADJ, L"IMAGE_REL_BASED_HIGHADJ" },
		{ IMAGE_REL_BASED_MACHINE_SPECIFIC_5, L"IMAGE_REL_BASED_MACHINE_SPECIFIC_5" },
		{ IMAGE_REL_BASED_RESERVED, L"IMAGE_REL_BASED_RESERVED" },
		{ IMAGE_REL_BASED_MACHINE_SPECIFIC_7, L"IMAGE_REL_BASED_MACHINE_SPECIFIC_7" },
		{ IMAGE_REL_BASED_MACHINE_SPECIFIC_8, L"IMAGE_REL_BASED_MACHINE_SPECIFIC_8" },
		{ IMAGE_REL_BASED_MACHINE_SPECIFIC_9, L"IMAGE_REL_BASED_MACHINE_SPECIFIC_9" },
		{ IMAGE_REL_BASED_DIR64, L"IMAGE_REL_BASED_DIR64" }
	};
	export struct PERELOC {
		DWORD                    dwOffset;     //File's raw offset of this Relocation descriptor.
		IMAGE_BASE_RELOCATION    stBaseReloc;  //Standard IMAGE_BASE_RELOCATION header.
		std::vector<PERELOCDATA> vecRelocData; //Array of the Relocation data struct.
	};
	export using PERELOC_VEC = std::vector<PERELOC>;


	//Debug table.
	export struct PEDEBUGDBGHDR {
		//dwHdr[6] is an array of the first six DWORDs of IMAGE_DEBUG_DIRECTORY::PointerToRawData data (Debug info header).
		//Their meaning varies depending on dwHdr[0] (Signature) value.
		//If dwHdr[0] == 0x53445352 (Ascii "RSDS") it's PDB 7.0 file:
		// Then dwHdr[1]-dwHdr[4] is GUID (*((GUID*)&dwHdr[1])). dwHdr[5] is Counter/Age.
		//If dwHdr[0] == 0x3031424E (Ascii "NB10") it's PDB 2.0 file:
		// Then dwHdr[1] is Offset. dwHdr[2] is Time/Signature. dwHdr[3] is Counter/Age.
		DWORD       dwHdr[6];
		std::string strPDBName; //PDB file name/path.
	};
	export struct PEDEBUG {
		DWORD                 dwOffset;       //File's raw offset of this Debug descriptor.
		IMAGE_DEBUG_DIRECTORY stDebugDir;     //Standard IMAGE_DEBUG_DIRECTORY header.
		PEDEBUGDBGHDR         stDebugHdrInfo; //Debug info header.
	};
	export using PEDEBUG_VEC = std::vector<PEDEBUG>;
	export inline const std::unordered_map<DWORD, std::wstring_view> MapDbgType {
		{ IMAGE_DEBUG_TYPE_UNKNOWN, L"IMAGE_DEBUG_TYPE_UNKNOWN" },
		{ IMAGE_DEBUG_TYPE_COFF, L"IMAGE_DEBUG_TYPE_COFF" },
		{ IMAGE_DEBUG_TYPE_CODEVIEW, L"IMAGE_DEBUG_TYPE_CODEVIEW" },
		{ IMAGE_DEBUG_TYPE_FPO, L"IMAGE_DEBUG_TYPE_FPO" },
		{ IMAGE_DEBUG_TYPE_MISC, L"IMAGE_DEBUG_TYPE_MISC" },
		{ IMAGE_DEBUG_TYPE_EXCEPTION, L"IMAGE_DEBUG_TYPE_EXCEPTION" },
		{ IMAGE_DEBUG_TYPE_FIXUP, L"IMAGE_DEBUG_TYPE_FIXUP" },
		{ IMAGE_DEBUG_TYPE_OMAP_TO_SRC, L"IMAGE_DEBUG_TYPE_OMAP_TO_SRC" },
		{ IMAGE_DEBUG_TYPE_OMAP_FROM_SRC, L"IMAGE_DEBUG_TYPE_OMAP_FROM_SRC" },
		{ IMAGE_DEBUG_TYPE_BORLAND, L"IMAGE_DEBUG_TYPE_BORLAND" },
		{ IMAGE_DEBUG_TYPE_RESERVED10, L"IMAGE_DEBUG_TYPE_RESERVED10" },
		{ IMAGE_DEBUG_TYPE_CLSID, L"IMAGE_DEBUG_TYPE_CLSID" },
		{ IMAGE_DEBUG_TYPE_VC_FEATURE, L"IMAGE_DEBUG_TYPE_VC_FEATURE" },
		{ IMAGE_DEBUG_TYPE_POGO, L"IMAGE_DEBUG_TYPE_POGO" },
		{ IMAGE_DEBUG_TYPE_ILTCG, L"IMAGE_DEBUG_TYPE_ILTCG" },
		{ IMAGE_DEBUG_TYPE_MPX, L"IMAGE_DEBUG_TYPE_MPX" },
		{ IMAGE_DEBUG_TYPE_REPRO, L"IMAGE_DEBUG_TYPE_REPRO" }
	};

	//TLS table.
	export struct PETLS {
		DWORD              dwOffset;          //File's raw offset of the TLS header descriptor.
		union UNPETLS {
			IMAGE_TLS_DIRECTORY32 stTLSDir32; //x86 standard TLS header.
			IMAGE_TLS_DIRECTORY64 stTLSDir64; //x64 TLS header.
		} unTLS;
		std::vector<DWORD> vecTLSCallbacks;   //Array of the TLS callbacks.
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapTLSCharact {
		{ IMAGE_SCN_ALIGN_1BYTES, L"IMAGE_SCN_ALIGN_1BYTES" },
		{ IMAGE_SCN_ALIGN_2BYTES, L"IMAGE_SCN_ALIGN_2BYTES" },
		{ IMAGE_SCN_ALIGN_4BYTES, L"IMAGE_SCN_ALIGN_4BYTES" },
		{ IMAGE_SCN_ALIGN_8BYTES, L"IMAGE_SCN_ALIGN_8BYTES" },
		{ IMAGE_SCN_ALIGN_16BYTES, L"IMAGE_SCN_ALIGN_16BYTES" },
		{ IMAGE_SCN_ALIGN_32BYTES, L"IMAGE_SCN_ALIGN_32BYTES" },
		{ IMAGE_SCN_ALIGN_64BYTES, L"IMAGE_SCN_ALIGN_64BYTES" },
		{ IMAGE_SCN_ALIGN_128BYTES, L"IMAGE_SCN_ALIGN_128BYTES" },
		{ IMAGE_SCN_ALIGN_256BYTES, L"IMAGE_SCN_ALIGN_256BYTES" },
		{ IMAGE_SCN_ALIGN_512BYTES, L"IMAGE_SCN_ALIGN_512BYTES" },
		{ IMAGE_SCN_ALIGN_1024BYTES, L"IMAGE_SCN_ALIGN_1024BYTES" },
		{ IMAGE_SCN_ALIGN_2048BYTES, L"IMAGE_SCN_ALIGN_2048BYTES" },
		{ IMAGE_SCN_ALIGN_4096BYTES, L"IMAGE_SCN_ALIGN_4096BYTES" },
		{ IMAGE_SCN_ALIGN_8192BYTES, L"IMAGE_SCN_ALIGN_8192BYTES" },
		{ IMAGE_SCN_ALIGN_MASK, L"IMAGE_SCN_ALIGN_MASK" }
	};

	//LoadConfigDirectory.
	export struct PELOADCONFIG {
		DWORD dwOffset;                            //File's raw offset of the LCD descriptor.
		union UNPELOADCONFIG {
			IMAGE_LOAD_CONFIG_DIRECTORY32 stLCD32; //x86 LCD descriptor.
			IMAGE_LOAD_CONFIG_DIRECTORY64 stLCD64; //x64 LCD descriptor.
		} unLCD;
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapLCDGuardFlags {
		{ IMAGE_GUARD_CF_INSTRUMENTED, L"IMAGE_GUARD_CF_INSTRUMENTED (Module performs control flow integrity checks using system-supplied support)" },
		{ IMAGE_GUARD_CFW_INSTRUMENTED, L"IMAGE_GUARD_CFW_INSTRUMENTED (Module performs control flow and write integrity checks)" },
		{ IMAGE_GUARD_CF_FUNCTION_TABLE_PRESENT, L"IMAGE_GUARD_CF_FUNCTION_TABLE_PRESENT (Module contains valid control flow target metadata)" },
		{ IMAGE_GUARD_SECURITY_COOKIE_UNUSED, L"IMAGE_GUARD_SECURITY_COOKIE_UNUSED (Module does not make use of the /GS security cookie)" },
		{ IMAGE_GUARD_PROTECT_DELAYLOAD_IAT, L"IMAGE_GUARD_PROTECT_DELAYLOAD_IAT (Module supports read only delay load IAT)" },
		{ IMAGE_GUARD_DELAYLOAD_IAT_IN_ITS_OWN_SECTION, L"IMAGE_GUARD_DELAYLOAD_IAT_IN_ITS_OWN_SECTION (Delayload import table in its own .didat section (with nothing else in it) that can be freely reprotected)" },
		{ IMAGE_GUARD_CF_EXPORT_SUPPRESSION_INFO_PRESENT, L"IMAGE_GUARD_CF_EXPORT_SUPPRESSION_INFO_PRESENT (Module contains suppressed export information. This also infers that the address taken IAT table is also present in the load config.)" },
		{ IMAGE_GUARD_CF_ENABLE_EXPORT_SUPPRESSION, L"IMAGE_GUARD_CF_ENABLE_EXPORT_SUPPRESSION (Module enables suppression of exports)" },
		{ IMAGE_GUARD_CF_LONGJUMP_TABLE_PRESENT, L"IMAGE_GUARD_CF_LONGJUMP_TABLE_PRESENT (Module contains longjmp target information)" },
		{ IMAGE_GUARD_RF_INSTRUMENTED, L"IMAGE_GUARD_RF_INSTRUMENTED (Module contains return flow instrumentation and metadata)" },
		{ IMAGE_GUARD_RF_ENABLE, L"IMAGE_GUARD_RF_ENABLE (Module requests that the OS enable return flow protection)" },
		{ IMAGE_GUARD_RF_STRICT, L"IMAGE_GUARD_RF_STRICT (Module requests that the OS enable return flow protection in strict mode)" },
		{ IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_MASK, L"IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_MASK (Stride of Guard CF function table encoded in these bits (additional count of bytes per element))" },
		{ IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_SHIFT, L"IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_SHIFT (Shift to right-justify Guard CF function table stride)" }
	};

	//Bound import table.
	export struct PEBOUNDFORWARDER {
		DWORD                     dwOffset;              //File's raw offset of this Bound Forwarder descriptor.
		IMAGE_BOUND_FORWARDER_REF stBoundForwarder;      //Standard IMAGE_BOUND_FORWARDER_REF struct.
		std::string               strBoundForwarderName; //Bound forwarder name.
	};
	export struct PEBOUNDIMPORT {
		DWORD                         dwOffset;          //File's raw offset of this Bound Import descriptor.
		IMAGE_BOUND_IMPORT_DESCRIPTOR stBoundImpDesc;    //Standard IMAGE_BOUND_IMPORT_DESCRIPTOR struct.
		std::string                   strBoundName;      //Bound Import name.
		std::vector<PEBOUNDFORWARDER> vecBoundForwarder; //Array of the Bound Forwarder structs.
	};
	export using PEBOUNDIMPORT_VEC = std::vector<PEBOUNDIMPORT>;

	//Delay import table.
	export struct PEDELAYIMPORTFUNC {
		union UNPEDELAYIMPORTTHUNK {
			struct x32 {
				IMAGE_THUNK_DATA32 stImportAddressTable;      //x86 Import Address Table struct.
				IMAGE_THUNK_DATA32 stImportNameTable;         //x86 Import Name Table struct.
				IMAGE_THUNK_DATA32 stBoundImportAddressTable; //x86 Bound Import Address Table struct.
				IMAGE_THUNK_DATA32 stUnloadInformationTable;  //x86 Unload Information Table struct.
			} st32;
			struct x64 {
				IMAGE_THUNK_DATA64 stImportAddressTable;      //x64 Import Address Table struct.
				IMAGE_THUNK_DATA64 stImportNameTable;         //x64 Import Name Table struct.
				IMAGE_THUNK_DATA64 stBoundImportAddressTable; //x64 Bound Import Address Table struct
				IMAGE_THUNK_DATA64 stUnloadInformationTable;  //x64 Unload Information Table struct.
			} st64;
		} unThunk;
		IMAGE_IMPORT_BY_NAME stImpByName; //Standard IMAGE_IMPORT_BY_NAME struct.
		std::string          strFuncName; //Function name.
	};
	export struct PEDELAYIMPORT {
		DWORD                          dwOffset;        //File's raw offset of this Delay Import descriptor.
		IMAGE_DELAYLOAD_DESCRIPTOR     stDelayImpDesc;  //Standard IMAGE_DELAYLOAD_DESCRIPTOR struct.
		std::string                    strModuleName;   //Import module name.
		std::vector<PEDELAYIMPORTFUNC> vecDelayImpFunc; //Array of the Delay Import module functions.
	};
	export using PEDELAYIMPORT_VEC = std::vector<PEDELAYIMPORT>;

	//COM descriptor table.
	export struct PECOMDESCRIPTOR {
		DWORD              dwOffset; //File's raw offset of the IMAGE_COR20_HEADER descriptor.
		IMAGE_COR20_HEADER stCorHdr; //Standard IMAGE_COR20_HEADER struct.
	};
	export inline const std::unordered_map<DWORD, std::wstring_view> MapCOR20Flags {
		{ ReplacesCorHdrNumericDefines::COMIMAGE_FLAGS_ILONLY, L"COMIMAGE_FLAGS_ILONLY" },
		{ ReplacesCorHdrNumericDefines::COMIMAGE_FLAGS_32BITREQUIRED, L"COMIMAGE_FLAGS_32BITREQUIRED" },
		{ ReplacesCorHdrNumericDefines::COMIMAGE_FLAGS_IL_LIBRARY, L"COMIMAGE_FLAGS_IL_LIBRARY" },
		{ ReplacesCorHdrNumericDefines::COMIMAGE_FLAGS_STRONGNAMESIGNED, L"COMIMAGE_FLAGS_STRONGNAMESIGNED" },
		{ ReplacesCorHdrNumericDefines::COMIMAGE_FLAGS_NATIVE_ENTRYPOINT, L"COMIMAGE_FLAGS_NATIVE_ENTRYPOINT" },
		{ ReplacesCorHdrNumericDefines::COMIMAGE_FLAGS_TRACKDEBUGDATA, L"COMIMAGE_FLAGS_TRACKDEBUGDATA" },
		{ ReplacesCorHdrNumericDefines::COMIMAGE_FLAGS_32BITPREFERRED, L"COMIMAGE_FLAGS_32BITPREFERRED" }
	};

	export enum class EFileType : std::uint8_t {
		UNKNOWN = 0, PE32, PE64, PEROM
	};

	//Return codes.
	export constexpr auto PEOK = 0x0;
	export constexpr auto ERR_FILE_OPEN = 0x01;
	export constexpr auto ERR_FILE_SIZESMALL = 0x02;
	export constexpr auto ERR_FILE_MAPPING = 0x03;
	export constexpr auto ERR_FILE_NODOSHDR = 0x04;

	//Internal helper method to check overflow of addition.
	[[nodiscard]] constexpr auto IsSumOverflow(DWORD_PTR dwFirst, DWORD_PTR dwSecond) -> bool
	{
		return (dwFirst + dwSecond) < dwFirst;
	}

	export class Clibpe final
	{
	public:
		Clibpe() = default;
		Clibpe(const wchar_t* pwszFile);
		Clibpe(const Clibpe&) = delete;
		Clibpe(Clibpe&&) = delete;
		~Clibpe();
		auto OpenFile(const wchar_t* pwszFile) -> int;
		auto OpenFile(std::span<const std::byte> spnFile) -> int;
		void CloseFile();
		[[nodiscard]] auto GetDOSHeader()const->std::optional<IMAGE_DOS_HEADER>;
		[[nodiscard]] auto GetRichHeader()const->std::optional<PERICHHDR_VEC>;
		[[nodiscard]] auto GetNTHeader()const->std::optional<PENTHDR>;
		[[nodiscard]] auto GetDataDirs()const->std::optional<PEDATADIR_VEC>;
		[[nodiscard]] auto GetSecHeaders()const->std::optional<PESECHDR_VEC>;
		[[nodiscard]] auto GetExport()const->std::optional<PEEXPORT>;
		[[nodiscard]] auto GetImport()const->std::optional<PEIMPORT_VEC>;
		[[nodiscard]] auto GetResources()const->std::optional<PERESROOT>;
		[[nodiscard]] auto GetExceptions()const->std::optional<PEEXCEPTION_VEC>;
		[[nodiscard]] auto GetSecurity()const->std::optional<PESECURITY_VEC>;
		[[nodiscard]] auto GetRelocations()const->std::optional<PERELOC_VEC>;
		[[nodiscard]] auto GetDebug()const->std::optional<PEDEBUG_VEC>;
		[[nodiscard]] auto GetTLS()const->std::optional<PETLS>;
		[[nodiscard]] auto GetLoadConfig()const->std::optional<PELOADCONFIG>;
		[[nodiscard]] auto GetBoundImport()const->std::optional<PEBOUNDIMPORT_VEC>;
		[[nodiscard]] auto GetDelayImport()const->std::optional<PEDELAYIMPORT_VEC>;
		[[nodiscard]] auto GetCOMDescriptor()const->std::optional<PECOMDESCRIPTOR>;
	private:
		[[nodiscard]] auto GetFileSize()const->ULONGLONG;
		[[nodiscard]] auto GetBaseAddr()const->DWORD_PTR;
		[[nodiscard]] auto GetDosPtr()const->const IMAGE_DOS_HEADER*;
		[[nodiscard]] auto GetDirEntryRVA(DWORD dwEntry)const->DWORD;
		[[nodiscard]] auto GetDirEntrySize(DWORD dwEntry)const->DWORD;
		[[nodiscard]] auto GetImageBase()const->ULONGLONG;
		[[nodiscard]] auto GetSecHdrFromName(LPCSTR lpszName)const->PIMAGE_SECTION_HEADER;
		[[nodiscard]] auto GetSecHdrFromRVA(ULONGLONG ullRVA)const->PIMAGE_SECTION_HEADER;
		template<typename T>
		[[nodiscard]] auto GetTData(ULONGLONG ullOffset)const->T;
		template<typename T>
		[[nodiscard]] auto IsPtrSafe(T tAddr, bool fCanReferenceBoundary = false)const->bool;
		[[nodiscard]] auto PtrToOffset(LPCVOID lp)const->DWORD;
		[[nodiscard]] auto RVAToPtr(ULONGLONG ullRVA)const->LPVOID;
		bool ParseDOSHeader();
		bool ParseNTFileOptHeader();
	private:
		std::span<const std::byte> m_spnData { }; //File data.
		PIMAGE_NT_HEADERS32 m_pNTHeader32 { };    //NT header for x86.
		PIMAGE_NT_HEADERS64 m_pNTHeader64 { };    //NT header for x64.
		EFileType m_enPEType { }; //PE type: x64 or x86.
		HANDLE m_hFile { };       //Opened file handle.
		HANDLE m_hFileMap { };    //File-mapping handle.
		LPVOID m_pFileView { };   //File mapping-view handle.
		bool m_fOpened { };       //Is file successfully opened (by any OpenFile method)?
		bool m_fFileHandle { };   //Was file handle opened (by OpenFile(const wchar_t* pwszFile))?
		bool m_fHasNTHdr { };     //Does file have at least NT header.
	};

	Clibpe::Clibpe(const wchar_t* pwszFile)
	{
		OpenFile(pwszFile);
	}

	Clibpe::~Clibpe()
	{
		CloseFile();
	}

	auto Clibpe::OpenFile(const wchar_t* pwszFile)->int
	{
		assert(pwszFile != nullptr);
		if (m_fFileHandle) {
			CloseFile();
		}

		m_hFile = CreateFileW(pwszFile, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		assert(m_hFile != INVALID_HANDLE_VALUE);
		if (m_hFile == INVALID_HANDLE_VALUE)
			return ERR_FILE_OPEN;

		LARGE_INTEGER stLI { };
		if (GetFileSizeEx(m_hFile, &stLI) == FALSE || stLI.QuadPart < sizeof(IMAGE_DOS_HEADER)) {
			CloseHandle(m_hFile);
			return ERR_FILE_SIZESMALL;
		}

		m_hFileMap = CreateFileMappingW(m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
		assert(m_hFileMap != nullptr);
		if (m_hFileMap == nullptr) {
			CloseHandle(m_hFile);
			return ERR_FILE_MAPPING;
		}

		m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, 0);
		assert(m_pFileView != nullptr); //Not enough memory? File is too big?
		if (m_pFileView == nullptr) {
			CloseHandle(m_hFileMap);
			CloseHandle(m_hFile);
			return ERR_FILE_MAPPING;
		}

		m_fFileHandle = true;

		return OpenFile({ static_cast<std::byte*>(m_pFileView), static_cast<std::size_t>(stLI.QuadPart) });
	}

	auto Clibpe::OpenFile(std::span<const std::byte> spnFile)->int
	{
		assert(!spnFile.empty());
		if (m_fOpened) {
			CloseFile();
		}

		if (spnFile.size() < sizeof(IMAGE_DOS_HEADER))
			return ERR_FILE_SIZESMALL;

		m_spnData = spnFile;

		if (!ParseDOSHeader()) {
			CloseFile();
			return ERR_FILE_NODOSHDR;
		}

		m_fOpened = true;
		ParseNTFileOptHeader();

		return PEOK;
	}

	void Clibpe::CloseFile()
	{
		if (m_fFileHandle) {
			UnmapViewOfFile(m_pFileView);
			CloseHandle(m_hFileMap);
			CloseHandle(m_hFile);
		}
		m_hFile = nullptr;
		m_hFileMap = nullptr;
		m_pFileView = nullptr;
		m_pNTHeader32 = nullptr;
		m_pNTHeader64 = nullptr;
		m_enPEType = { };
		m_spnData = { };
		m_fHasNTHdr = false;
		m_fFileHandle = false;
		m_fOpened = false;
	}

	auto Clibpe::GetDOSHeader()const->std::optional<IMAGE_DOS_HEADER>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		return *GetDosPtr();
	}

	auto Clibpe::GetRichHeader()const->std::optional<PERICHHDR_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		//Undocumented, so called «Rich» header, dwells not in all PE files.
		//«Rich» stub starts at 0x80 offset, before pDosHdr->e_lfanew (PE header offset start).
		//If e_lfanew <= 0x80 — there is no «Rich» header.

		const auto ullBaseAddr = GetBaseAddr();
		const auto e_lfanew = GetDosPtr()->e_lfanew;
		if (e_lfanew <= 0x80 || !IsPtrSafe(ullBaseAddr + static_cast<DWORD_PTR>(e_lfanew)))
			return std::nullopt;

		const auto pRichStartVA = reinterpret_cast<PDWORD>(ullBaseAddr + 0x80);
		auto pRichIter = pRichStartVA;

		for (auto i = 0; i < ((e_lfanew - 0x80) / 4); ++i, ++pRichIter) {
			//Check "Rich" (ANSI) sign, it's always at the end of the «Rich» header.
			//Then take DWORD right after the "Rich" sign — it's a xor mask.
			//Apply this mask to the first DWORD of «Rich» header, it must be "DanS" (ANSI) after xoring.
			if ((*pRichIter == 0x68636952/*"Rich"*/) && ((*pRichStartVA ^ *(pRichIter + 1)) == 0x536E6144/*"Dans"*/)
				&& (reinterpret_cast<DWORD_PTR>(pRichIter) >= ullBaseAddr + 0x90)) { //To avoid too small (bogus) «Rich» header.
				//Amount of all «Rich» DOUBLE_DWORD structs.
				//First 16 bytes in «Rich» header are irrelevant. It's "DanS" itself and 12 more zeroed bytes.
				//That's why we subtracting 0x90 to find out amount of all «Rich» structures:
				//0x80 («Rich» start) + 16 (0xF) = 0x90.
				const DWORD dwRichSize = static_cast<DWORD>((reinterpret_cast<DWORD_PTR>(pRichIter) - ullBaseAddr) - 0x90) / 8;
				const DWORD dwRichXORMask = *(pRichIter + 1); //xor mask of «Rich» header.
				pRichIter = reinterpret_cast<PDWORD>(ullBaseAddr + 0x90);//VA of «Rich» DOUBLE_DWORD structs start.

				PERICHHDR_VEC vecRichHdr;
				for (auto j = 0UL; j < dwRichSize; ++j) {
					//Pushing double DWORD of «Rich» structure.
					//Disassembling first DWORD by two WORDs.
					vecRichHdr.emplace_back(static_cast<DWORD>(reinterpret_cast<DWORD_PTR>(pRichIter) - GetBaseAddr()),
						HIWORD(dwRichXORMask ^ *pRichIter), LOWORD(dwRichXORMask ^ *pRichIter), dwRichXORMask ^ *(pRichIter + 1));
					pRichIter += 2; //Jump to the next DOUBLE_DWORD.
				}
				return vecRichHdr;
			}
		}

		return std::nullopt;
	}

	auto Clibpe::GetNTHeader()const->std::optional<PENTHDR>
	{
		assert(m_fOpened);
		if (!m_fOpened || !m_fHasNTHdr)
			return std::nullopt;

		PENTHDR stNTHdr;
		switch (m_enPEType) {
		case EFileType::PE32:
			stNTHdr.unHdr.stNTHdr32 = *m_pNTHeader32;
			stNTHdr.dwOffset = PtrToOffset(m_pNTHeader32);
			break;
		case EFileType::PE64:
			stNTHdr.unHdr.stNTHdr64 = *m_pNTHeader64;
			stNTHdr.dwOffset = PtrToOffset(m_pNTHeader64);
			break;
		default:
			return std::nullopt;
		}

		return stNTHdr;
	}

	auto Clibpe::GetDataDirs()const->std::optional<PEDATADIR_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened || !m_fHasNTHdr)
			return std::nullopt;

		PIMAGE_DATA_DIRECTORY pDataDir;
		DWORD dwRVAAndSizes;

		switch (m_enPEType) {
		case EFileType::PE32:
			pDataDir = reinterpret_cast<PIMAGE_DATA_DIRECTORY>(m_pNTHeader32->OptionalHeader.DataDirectory);
			dwRVAAndSizes = m_pNTHeader32->OptionalHeader.NumberOfRvaAndSizes;
			break;
		case EFileType::PE64:
			pDataDir = reinterpret_cast<PIMAGE_DATA_DIRECTORY>(m_pNTHeader64->OptionalHeader.DataDirectory);
			dwRVAAndSizes = m_pNTHeader64->OptionalHeader.NumberOfRvaAndSizes;
			break;
		default:
			return std::nullopt;
		}

		PEDATADIR_VEC vecDataDirs;
		for (auto iDir { 0U }; iDir < (dwRVAAndSizes > 15 ? 15 : dwRVAAndSizes); ++iDir, ++pDataDir) {
			std::string strSecName;
			if (const auto pSecHdr = GetSecHdrFromRVA(pDataDir->VirtualAddress);
				pSecHdr != nullptr && iDir != IMAGE_DIRECTORY_ENTRY_SECURITY) { //RVA of IMAGE_DIRECTORY_ENTRY_SECURITY is the file RAW offset.
				strSecName.assign(reinterpret_cast<char* const>(pSecHdr->Name), 8);
			}
			vecDataDirs.emplace_back(*pDataDir, std::move(strSecName));
		}

		return vecDataDirs.empty() ? std::nullopt : std::optional<PEDATADIR_VEC>(std::move(vecDataDirs));
	}

	auto Clibpe::GetSecHeaders()const->std::optional<PESECHDR_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened || !m_fHasNTHdr)
			return std::nullopt;

		PIMAGE_SECTION_HEADER pSecHdr;
		WORD wNumSections;
		DWORD dwSymbolTable;
		DWORD dwNumberOfSymbols;

		switch (m_enPEType) {
		case EFileType::PE32:
			pSecHdr = IMAGE_FIRST_SECTION(m_pNTHeader32);
			wNumSections = m_pNTHeader32->FileHeader.NumberOfSections;
			dwSymbolTable = m_pNTHeader32->FileHeader.PointerToSymbolTable;
			dwNumberOfSymbols = m_pNTHeader32->FileHeader.NumberOfSymbols;
			break;
		case EFileType::PE64:
			pSecHdr = IMAGE_FIRST_SECTION(m_pNTHeader64);
			wNumSections = m_pNTHeader64->FileHeader.NumberOfSections;
			dwSymbolTable = m_pNTHeader64->FileHeader.PointerToSymbolTable;
			dwNumberOfSymbols = m_pNTHeader64->FileHeader.NumberOfSymbols;
			break;
		default:
			return std::nullopt;
		}

		PESECHDR_VEC vecSecHeaders;
		vecSecHeaders.reserve(wNumSections);

		for (unsigned i = 0; i < wNumSections; ++i, ++pSecHdr) {
			if (!IsPtrSafe(reinterpret_cast<DWORD_PTR>(pSecHdr) + sizeof(IMAGE_SECTION_HEADER)))
				break;

			std::string strSecRealName;
			if (pSecHdr->Name[0] == '/') {	//Deprecated, but still used "feature" of section name.
				//https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-image_section_header
				//«An 8-byte, null-padded UTF-8 string. There is no terminating null character 
				//if the string is exactly eight characters long.
				//For longer names, this member contains a forward slash (/) followed by an ASCII representation 
				//of a decimal number that is an offset into the string table.»
				//String Table dwells right after the end of Symbol Table.
				//Each symbol in Symbol Table occupies exactly 18 bytes.
				//So String Table's beginning can be calculated like this:
				//FileHeader.PointerToSymbolTable + FileHeader.NumberOfSymbols * 18;

				const auto pStart = reinterpret_cast<const char*>(&pSecHdr->Name[1]);
				char* pEnd { };
				errno = 0;
				const auto lOffset = strtol(pStart, &pEnd, 10);
				if (pEnd == pStart || errno == ERANGE)
					continue; //Going next section entry.

				const auto lpszSecRealName = reinterpret_cast<const char*>(GetBaseAddr()
					+ static_cast<DWORD_PTR>(dwSymbolTable) + static_cast<DWORD_PTR>(dwNumberOfSymbols) * 18
					+ static_cast<DWORD_PTR>(lOffset));
				if (IsPtrSafe(lpszSecRealName)) {
					strSecRealName = lpszSecRealName;
				}
			}

			vecSecHeaders.emplace_back(PtrToOffset(pSecHdr), *pSecHdr, std::move(strSecRealName));
		}

		return vecSecHeaders.empty() ? std::nullopt : std::optional<PESECHDR_VEC>(std::move(vecSecHeaders));
	}

	auto Clibpe::GetExport()const->std::optional<PEEXPORT>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		const auto dwExportStartRVA = GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_EXPORT);
		const auto dwExportEndRVA = dwExportStartRVA + GetDirEntrySize(IMAGE_DIRECTORY_ENTRY_EXPORT);
		const auto pExportDir = static_cast<PIMAGE_EXPORT_DIRECTORY>(RVAToPtr(dwExportStartRVA));
		if (pExportDir == nullptr)
			return std::nullopt;

		const auto pdwFuncsRVA = static_cast<PDWORD>(RVAToPtr(pExportDir->AddressOfFunctions));
		if (pdwFuncsRVA == nullptr)
			return std::nullopt;

		const auto pwOrdinals = static_cast<PWORD>(RVAToPtr(pExportDir->AddressOfNameOrdinals));
		const auto pdwNamesRVA = static_cast<PDWORD>(RVAToPtr(pExportDir->AddressOfNames));
		std::vector<PEEXPORTFUNC> vecFuncs;

		try {
			for (std::size_t iterFuncs = 0; iterFuncs < static_cast<std::size_t>(pExportDir->NumberOfFunctions); ++iterFuncs) {
				if (!IsPtrSafe(pdwFuncsRVA + iterFuncs)) //Checking pdwFuncsRVA array.
					break;

				if (pdwFuncsRVA[iterFuncs]) { //if RVA==0 —> going next entry.
					std::string strFuncName;
					std::string strForwarderName;
					DWORD dwNameRVA { };
					if (pdwNamesRVA && pwOrdinals) {
						for (std::size_t iterFuncNames = 0; iterFuncNames < static_cast<std::size_t>(pExportDir->NumberOfNames); ++iterFuncNames) {
							if (!IsPtrSafe(pwOrdinals + iterFuncNames)) //Checking pwOrdinals array.
								break;

							if (pwOrdinals[iterFuncNames] == iterFuncs) { //Cycling through ordinals table to get func name.
								dwNameRVA = pdwNamesRVA[iterFuncNames];
								if (const auto pszFuncName = static_cast<LPCSTR>(RVAToPtr(dwNameRVA)); //Checking func name for length correctness.
									pszFuncName && (StringCchLengthA(pszFuncName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
									strFuncName = pszFuncName;
								}
								break;
							}
						}
					}

					if ((pdwFuncsRVA[iterFuncs] >= dwExportStartRVA) && (pdwFuncsRVA[iterFuncs] <= dwExportEndRVA)) {
						if (const auto pszForwarderName = static_cast<LPCSTR>(RVAToPtr(pdwFuncsRVA[iterFuncs])); //Checking forwarder name for length correctness.
							pszForwarderName && (StringCchLengthA(pszForwarderName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
							strForwarderName = pszForwarderName;
						}
					}
					vecFuncs.emplace_back(pdwFuncsRVA[iterFuncs], static_cast<DWORD>(iterFuncs)/*Ordinal*/, dwNameRVA,
						std::move(strFuncName), std::move(strForwarderName));
				}
			}

			std::string strModuleName; //Actual IMG name.
			if (const auto szExportName = static_cast<LPCSTR>(RVAToPtr(pExportDir->Name)); //Checking Export name for length correctness.
				szExportName && (StringCchLengthA(szExportName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
				strModuleName = szExportName;
			}

			return std::make_optional<PEEXPORT>(PtrToOffset(pExportDir), *pExportDir, std::move(strModuleName), std::move(vecFuncs));
		}
		catch (const std::bad_alloc&) {
			vecFuncs.clear();
			MessageBoxW(nullptr, L"E_OUTOFMEMORY error while trying to get Export table.\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}
		catch (...) {
			MessageBoxW(nullptr, L"Unknown exception raised while trying to get Export table.\r\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}

		return std::nullopt;
	}

	auto Clibpe::GetImport()const->std::optional<PEIMPORT_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		auto pImpDesc = static_cast<PIMAGE_IMPORT_DESCRIPTOR>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_IMPORT)));
		if (pImpDesc == nullptr)
			return std::nullopt;

		PEIMPORT_VEC vecImport;
		try {
			//Counter for import modules. If it exceeds iMaxModules we stop parsing file, it's definitely bogus.
			//Very unlikely PE file has more than 1000 import modules.
			constexpr auto iMaxModules = 1000;
			constexpr auto iMaxFuncs = 5000;
			int iModulesCount = 0;

			if (m_enPEType == EFileType::PE32) {
				while (pImpDesc->Name) {
					auto pThunk32 = reinterpret_cast<PIMAGE_THUNK_DATA32>(static_cast<DWORD_PTR>(pImpDesc->OriginalFirstThunk));
					if (!pThunk32) {
						pThunk32 = reinterpret_cast<PIMAGE_THUNK_DATA32>(static_cast<DWORD_PTR>(pImpDesc->FirstThunk));
					}

					if (pThunk32) {
						pThunk32 = static_cast<PIMAGE_THUNK_DATA32>(RVAToPtr(reinterpret_cast<DWORD_PTR>(pThunk32)));
						if (!pThunk32)
							break;

						std::vector<PEIMPORTFUNC> vecFunc;
						std::string strDllName;
						//Counter for import module funcs, if it exceeds iMaxFuncs we stop parsing import descr, it's definitely bogus.
						int iFuncsCount = 0;

						while (pThunk32->u1.AddressOfData) {
							PEIMPORTFUNC::UNPEIMPORTTHUNK unImpThunk32;
							unImpThunk32.stThunk32 = *pThunk32;
							IMAGE_IMPORT_BY_NAME stImpByName { };
							std::string strFuncName;
							if (!(pThunk32->u1.Ordinal & IMAGE_ORDINAL_FLAG32)) {
								if (const auto pName = static_cast<PIMAGE_IMPORT_BY_NAME>(RVAToPtr(pThunk32->u1.AddressOfData));
									pName && (StringCchLengthA(pName->Name, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
									stImpByName = *pName;
									strFuncName = pName->Name;
								}
							}
							vecFunc.emplace_back(unImpThunk32, stImpByName, std::move(strFuncName));

							if (!IsPtrSafe(++pThunk32))
								break;
							if (++iFuncsCount == iMaxFuncs)
								break;
						}

						if (const auto szName = static_cast<LPCSTR>(RVAToPtr(pImpDesc->Name));
							szName && (StringCchLengthA(szName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
							strDllName = szName;
						}

						vecImport.emplace_back(PtrToOffset(pImpDesc), *pImpDesc, std::move(strDllName), std::move(vecFunc));

						if (!IsPtrSafe(++pImpDesc))
							break;
					}
					else { //No IMPORT pointers for that DLL?...
						if (!IsPtrSafe(++pImpDesc))  //Going next dll.
							break;
					}

					if (++iModulesCount == iMaxModules)
						break;
				}
			}
			else if (m_enPEType == EFileType::PE64) {
				while (pImpDesc->Name) {
					auto pThunk64 = reinterpret_cast<PIMAGE_THUNK_DATA64>(static_cast<DWORD_PTR>(pImpDesc->OriginalFirstThunk));
					if (!pThunk64) {
						pThunk64 = reinterpret_cast<PIMAGE_THUNK_DATA64>(static_cast<DWORD_PTR>(pImpDesc->FirstThunk));
					}

					if (pThunk64) {
						pThunk64 = static_cast<PIMAGE_THUNK_DATA64>(RVAToPtr(reinterpret_cast<DWORD_PTR>(pThunk64)));
						if (!pThunk64)
							break;

						std::vector<PEIMPORTFUNC> vecFunc;
						std::string strDllName;
						int iFuncsCount = 0;

						while (pThunk64->u1.AddressOfData) {
							PEIMPORTFUNC::UNPEIMPORTTHUNK unImpThunk64;
							unImpThunk64.stThunk64 = *pThunk64;
							IMAGE_IMPORT_BY_NAME stImpByName { };
							std::string strFuncName;
							if (!(pThunk64->u1.Ordinal & IMAGE_ORDINAL_FLAG64)) {
								if (const auto pName = static_cast<PIMAGE_IMPORT_BY_NAME>(RVAToPtr(pThunk64->u1.AddressOfData));
									pName && (StringCchLengthA(pName->Name, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
									stImpByName = *pName;
									strFuncName = pName->Name;
								}
							}
							vecFunc.emplace_back(unImpThunk64, stImpByName, std::move(strFuncName));

							if (!IsPtrSafe(++pThunk64))
								break;
							if (++iFuncsCount == iMaxFuncs)
								break;
						}

						if (const auto szName = static_cast<LPCSTR>(RVAToPtr(pImpDesc->Name));
							szName && (StringCchLengthA(szName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
							strDllName = szName;
						}

						vecImport.emplace_back(PtrToOffset(pImpDesc), *pImpDesc, std::move(strDllName), std::move(vecFunc));

						if (!IsPtrSafe(++pImpDesc))
							break;
					}
					else {
						if (!IsPtrSafe(++pImpDesc))
							break;
					}

					if (++iModulesCount == iMaxModules)
						break;
				}
			}

			return vecImport.empty() ? std::nullopt : std::optional<PEIMPORT_VEC>(std::move(vecImport));
		}
		catch (const std::bad_alloc&) {
			vecImport.clear();
			MessageBoxW(nullptr, L"E_OUTOFMEMORY error while trying to get Import table.\r\n"
				L"Too many import entries!\nFile seems to be corrupted.", L"Error", MB_ICONERROR);
		}
		catch (...) {
			MessageBoxW(nullptr, L"Unknown exception raised while trying to get Import table.\r\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}

		return std::nullopt;
	}

	auto Clibpe::GetResources()const->std::optional<PERESROOT>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		const auto pResDirRoot = static_cast<PIMAGE_RESOURCE_DIRECTORY>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_RESOURCE)));
		if (pResDirRoot == nullptr)
			return std::nullopt;

		auto pResDirEntryRoot = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(pResDirRoot + 1);
		if (!IsPtrSafe(pResDirEntryRoot))
			return std::nullopt;

		std::vector<PERESROOTDATA> vecResDataRoot;
		try {
			const DWORD dwNumOfEntriesRoot = pResDirRoot->NumberOfNamedEntries + pResDirRoot->NumberOfIdEntries;
			if (!IsPtrSafe(pResDirEntryRoot + dwNumOfEntriesRoot))
				return std::nullopt;

			vecResDataRoot.reserve(dwNumOfEntriesRoot);
			for (unsigned iLvLRoot = 0; iLvLRoot < dwNumOfEntriesRoot; ++iLvLRoot) {
				PIMAGE_RESOURCE_DATA_ENTRY pResDataEntryRoot { };
				std::wstring wstrResNameRoot;
				std::vector<std::byte> vecRawResDataRoot;
				PERESLVL2 stResLvL2 { };

				if (pResDirEntryRoot->NameIsString) { //Name of Resource Type (ICON, BITMAP, MENU, etc...).
					if (IsSumOverflow(reinterpret_cast<DWORD_PTR>(pResDirRoot), static_cast<DWORD_PTR>(pResDirEntryRoot->NameOffset)))
						break;

					if (const auto pResDirStr = reinterpret_cast<PIMAGE_RESOURCE_DIR_STRING_U>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
						+ static_cast<DWORD_PTR>(pResDirEntryRoot->NameOffset)); IsPtrSafe(pResDirStr)) {
						//Copy not more then MAX_PATH chars into wstrResNameRoot, avoiding overflow.
						wstrResNameRoot.assign(pResDirStr->NameString, pResDirStr->Length < MAX_PATH ? pResDirStr->Length : MAX_PATH);
					}
				}

				if (pResDirEntryRoot->DataIsDirectory) {
					const auto pResDirLvL2 = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
						+ static_cast<DWORD_PTR>(pResDirEntryRoot->OffsetToDirectory));
					if (!IsPtrSafe(pResDirLvL2))
						break;

					std::vector<PERESLVL2DATA> vecResDataLvL2;
					if (pResDirLvL2 == pResDirRoot) { //Resource loop hack.
						stResLvL2 = { PtrToOffset(pResDirLvL2), *pResDirLvL2, vecResDataLvL2 };
					}
					else {
						auto pResDirEntryLvL2 = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(pResDirLvL2 + 1);
						const DWORD dwNumOfEntriesLvL2 = pResDirLvL2->NumberOfNamedEntries + pResDirLvL2->NumberOfIdEntries;
						if (!IsPtrSafe(pResDirEntryLvL2 + dwNumOfEntriesLvL2))
							break;

						vecResDataLvL2.reserve(dwNumOfEntriesLvL2);
						for (unsigned iLvL2 = 0; iLvL2 < dwNumOfEntriesLvL2; ++iLvL2) {
							PIMAGE_RESOURCE_DATA_ENTRY pResDataEntryLvL2 { };
							std::wstring wstrResNameLvL2;
							std::vector<std::byte> vecRawResDataLvL2;
							PERESLVL3 stResLvL3 { };

							if (pResDirEntryLvL2->NameIsString) { //Name of resource itself if not presented by ID ("AFX_MY_SUPER_DIALOG"...).
								if (IsSumOverflow(reinterpret_cast<DWORD_PTR>(pResDirRoot), static_cast<DWORD_PTR>(pResDirEntryLvL2->NameOffset)))
									break;

								if (const auto pResDirStr2 = reinterpret_cast<PIMAGE_RESOURCE_DIR_STRING_U>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
									+ static_cast<DWORD_PTR>(pResDirEntryLvL2->NameOffset)); IsPtrSafe(pResDirStr2)) {
									//Copy not more then MAX_PATH chars into wstrResNameLvL2, avoiding overflow.
									wstrResNameLvL2.assign(pResDirStr2->NameString, pResDirStr2->Length < MAX_PATH ? pResDirStr2->Length : MAX_PATH);
								}
							}

							if (pResDirEntryLvL2->DataIsDirectory) {
								const auto pResDirLvL3 = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
									+ static_cast<DWORD_PTR>(pResDirEntryLvL2->OffsetToDirectory));
								if (!IsPtrSafe(pResDirLvL3))
									break;

								std::vector<PERESLVL3DATA> vecResDataLvL3;
								if (pResDirLvL3 == pResDirLvL2 || pResDirLvL3 == pResDirRoot) {
									stResLvL3 = { PtrToOffset(pResDirLvL3), *pResDirLvL3, vecResDataLvL3 };
								}
								else {
									auto pResDirEntryLvL3 = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(pResDirLvL3 + 1);
									const DWORD dwNumOfEntriesLvL3 = pResDirLvL3->NumberOfNamedEntries + pResDirLvL3->NumberOfIdEntries;
									if (!IsPtrSafe(pResDirEntryLvL3 + dwNumOfEntriesLvL3))
										break;

									vecResDataLvL3.reserve(dwNumOfEntriesLvL3);
									for (unsigned iLvL3 = 0; iLvL3 < dwNumOfEntriesLvL3; ++iLvL3) {
										std::wstring wstrResNameLvL3;
										std::vector<std::byte> vecRawResDataLvL3;

										if (pResDirEntryLvL3->NameIsString) {
											if (IsSumOverflow(reinterpret_cast<DWORD_PTR>(pResDirRoot), static_cast<DWORD_PTR>(pResDirEntryLvL3->NameOffset)))
												break;

											if (const auto pResDirStr3 = reinterpret_cast<PIMAGE_RESOURCE_DIR_STRING_U>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
												+ static_cast<DWORD_PTR>(pResDirEntryLvL3->NameOffset)); IsPtrSafe(pResDirStr3)) {
												//Copy not more then MAX_PATH chars into wstrResNameLvL3, avoiding overflow.
												wstrResNameLvL3.assign(pResDirStr3->NameString, pResDirStr3->Length < MAX_PATH ? pResDirStr3->Length : MAX_PATH);
											}
										}

										const auto pResDataEntryLvL3 = reinterpret_cast<PIMAGE_RESOURCE_DATA_ENTRY>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
											+ static_cast<DWORD_PTR>(pResDirEntryLvL3->OffsetToData));
										if (IsPtrSafe(pResDataEntryLvL3)) {	//Resource LvL 3 RAW Data.
											//IMAGE_RESOURCE_DATA_ENTRY::OffsetToData is actually a general RVA,
											//not an offset from root IMAGE_RESOURCE_DIRECTORY, like IMAGE_RESOURCE_DIRECTORY_ENTRY::OffsetToData.

											//Checking RAW Resource data pointer out of bounds.
											if (const auto pThirdResRawDataBegin = static_cast<std::byte*>(RVAToPtr(pResDataEntryLvL3->OffsetToData));
												pThirdResRawDataBegin && IsPtrSafe(reinterpret_cast<DWORD_PTR>(pThirdResRawDataBegin)
													+ static_cast<DWORD_PTR>(pResDataEntryLvL3->Size), true)) {
												vecRawResDataLvL3.assign(pThirdResRawDataBegin, pThirdResRawDataBegin + pResDataEntryLvL3->Size);
											}
										}

										vecResDataLvL3.emplace_back(*pResDirEntryLvL3, std::move(wstrResNameLvL3),
											IsPtrSafe(pResDataEntryLvL3) ? *pResDataEntryLvL3 : IMAGE_RESOURCE_DATA_ENTRY { }, std::move(vecRawResDataLvL3));

										if (!IsPtrSafe(++pResDirEntryLvL3))
											break;
									}
									stResLvL3 = { PtrToOffset(pResDirLvL3), *pResDirLvL3, std::move(vecResDataLvL3) };
								}
							}
							else { //////Resource LvL2 RAW Data.
								pResDataEntryLvL2 = reinterpret_cast<PIMAGE_RESOURCE_DATA_ENTRY>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
									+ static_cast<DWORD_PTR>(pResDirEntryLvL2->OffsetToData));
								if (IsPtrSafe(pResDataEntryLvL2)) {
									//Checking RAW Resource data pointer out of bounds.
									if (const auto pSecondResRawDataBegin = static_cast<std::byte*>(RVAToPtr(pResDataEntryLvL2->OffsetToData));
										pSecondResRawDataBegin && IsPtrSafe(reinterpret_cast<DWORD_PTR>(pSecondResRawDataBegin)
											+ static_cast<DWORD_PTR>(pResDataEntryLvL2->Size), true)) {
										vecRawResDataLvL2.assign(pSecondResRawDataBegin, pSecondResRawDataBegin + pResDataEntryLvL2->Size);
									}
								}
							}
							vecResDataLvL2.emplace_back(*pResDirEntryLvL2, std::move(wstrResNameLvL2),
								IsPtrSafe(pResDataEntryLvL2) ? *pResDataEntryLvL2 : IMAGE_RESOURCE_DATA_ENTRY { }, std::move(vecRawResDataLvL2), stResLvL3);

							if (!IsPtrSafe(++pResDirEntryLvL2))
								break;
						}
						stResLvL2 = { PtrToOffset(pResDirLvL2), *pResDirLvL2, std::move(vecResDataLvL2) };
					}
				}
				else { //Resource LvL Root RAW Data.
					pResDataEntryRoot = reinterpret_cast<PIMAGE_RESOURCE_DATA_ENTRY>(reinterpret_cast<DWORD_PTR>(pResDirRoot)
						+ static_cast<DWORD_PTR>(pResDirEntryRoot->OffsetToData));
					if (IsPtrSafe(pResDataEntryRoot)) {
						//Checking RAW Resource data pointer out of bounds.
						if (const auto pRootResRawDataBegin = static_cast<std::byte*>(RVAToPtr(pResDataEntryRoot->OffsetToData));
							pRootResRawDataBegin && IsPtrSafe(reinterpret_cast<DWORD_PTR>(pRootResRawDataBegin)
								+ static_cast<DWORD_PTR>(pResDataEntryRoot->Size), true)) {
							vecRawResDataRoot.assign(pRootResRawDataBegin, pRootResRawDataBegin + pResDataEntryRoot->Size);
						}
					}
				}
				vecResDataRoot.emplace_back(*pResDirEntryRoot, std::move(wstrResNameRoot),
					IsPtrSafe(pResDataEntryRoot) ? *pResDataEntryRoot : IMAGE_RESOURCE_DATA_ENTRY { }, std::move(vecRawResDataRoot), stResLvL2);

				if (!IsPtrSafe(++pResDirEntryRoot))
					break;
			}

			return std::make_optional<PERESROOT>(PtrToOffset(pResDirRoot), *pResDirRoot, std::move(vecResDataRoot));
		}
		catch (const std::bad_alloc&) {
			vecResDataRoot.clear();
			MessageBoxW(nullptr, L"E_OUTOFMEMORY error while trying to get Resource table.\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}
		catch (...) {
			MessageBoxW(nullptr, L"Unknown exception raised while trying to get Resource table.\r\n\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}

		return std::nullopt;
	}

	auto Clibpe::GetExceptions()const->std::optional<PEEXCEPTION_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		//IMAGE_RUNTIME_FUNCTION_ENTRY (without leading underscore) might have different typedef,
		//depending on defined platform, see winnt.h
		auto pRuntimeFuncsEntry = static_cast<_PIMAGE_RUNTIME_FUNCTION_ENTRY>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_EXCEPTION)));
		if (pRuntimeFuncsEntry == nullptr)
			return std::nullopt;

		const auto dwEntries = GetDirEntrySize(IMAGE_DIRECTORY_ENTRY_EXCEPTION) / static_cast<DWORD>(sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY));
		if (!dwEntries || !IsPtrSafe(reinterpret_cast<DWORD_PTR>(pRuntimeFuncsEntry) + static_cast<DWORD_PTR>(dwEntries)))
			return std::nullopt;

		PEEXCEPTION_VEC vecException;
		for (auto i = 0UL; i < dwEntries; ++i, ++pRuntimeFuncsEntry) {
			if (!IsPtrSafe(pRuntimeFuncsEntry))
				break;

			vecException.emplace_back(PtrToOffset(pRuntimeFuncsEntry), *pRuntimeFuncsEntry);
		}

		return vecException.empty() ? std::nullopt : std::optional<PEEXCEPTION_VEC>(std::move(vecException));
	}

	auto Clibpe::GetSecurity()const->std::optional<PESECURITY_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		const auto dwSecurityDirOffset = GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_SECURITY);
		const auto dwSecurityDirSize = GetDirEntrySize(IMAGE_DIRECTORY_ENTRY_SECURITY);
		if (!dwSecurityDirOffset || !dwSecurityDirSize)
			return std::nullopt;

		//Checks for bogus file offsets that can cause DWORD_PTR overflow.
		if (IsSumOverflow(static_cast<DWORD_PTR>(dwSecurityDirOffset), GetBaseAddr()))
			return std::nullopt;

		auto dwSecurityDirStartVA = GetBaseAddr() + static_cast<DWORD_PTR>(dwSecurityDirOffset);
		if (IsSumOverflow(dwSecurityDirStartVA, static_cast<DWORD_PTR>(dwSecurityDirSize)))
			return std::nullopt;

		const DWORD_PTR dwSecurityDirEndVA = dwSecurityDirStartVA + static_cast<DWORD_PTR>(dwSecurityDirSize);

		if (!IsPtrSafe(dwSecurityDirStartVA) || !IsPtrSafe(dwSecurityDirEndVA, true))
			return std::nullopt;

		PESECURITY_VEC vecSecurity;
		while (dwSecurityDirStartVA < dwSecurityDirEndVA) {
			auto pCertificate = reinterpret_cast<LPWIN_CERTIFICATE>(dwSecurityDirStartVA);
			const auto dwCertSize = pCertificate->dwLength - static_cast<DWORD>(offsetof(WIN_CERTIFICATE, bCertificate));
			if (!IsPtrSafe(dwSecurityDirStartVA + static_cast<DWORD_PTR>(dwCertSize)))
				break;

			vecSecurity.emplace_back(PtrToOffset(pCertificate), *pCertificate);

			//Get next certificate entry, all entries start at 8 aligned address.
			DWORD dwLength = pCertificate->dwLength;
			dwLength += (8 - (dwLength & 7)) & 7;
			dwSecurityDirStartVA = dwSecurityDirStartVA + static_cast<DWORD_PTR>(dwLength);
			if (!IsPtrSafe(dwSecurityDirStartVA))
				break;
		}

		return vecSecurity.empty() ? std::nullopt : std::optional<PESECURITY_VEC>(std::move(vecSecurity));
	}

	auto Clibpe::GetRelocations()const->std::optional<PERELOC_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		auto pBaseRelocDesc = static_cast<PIMAGE_BASE_RELOCATION>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_BASERELOC)));
		if (pBaseRelocDesc == nullptr)
			return std::nullopt;

		PERELOC_VEC vecRelocs;
		try {
			if (!pBaseRelocDesc->SizeOfBlock || !pBaseRelocDesc->VirtualAddress) {
				vecRelocs.emplace_back(PtrToOffset(pBaseRelocDesc), *pBaseRelocDesc, std::vector<PERELOCDATA> { });
			}

			while ((pBaseRelocDesc->SizeOfBlock) && (pBaseRelocDesc->VirtualAddress)) {
				if (pBaseRelocDesc->SizeOfBlock < sizeof(IMAGE_BASE_RELOCATION)) {
					vecRelocs.emplace_back(PtrToOffset(pBaseRelocDesc), *pBaseRelocDesc, std::vector<PERELOCDATA>{ });
					break;
				}

				//Amount of Reloc entries.
				DWORD dwNumRelocEntries = (pBaseRelocDesc->SizeOfBlock - static_cast<DWORD>(sizeof(IMAGE_BASE_RELOCATION))) / static_cast<DWORD>(sizeof(WORD));
				auto pwRelocEntry = reinterpret_cast<PWORD>(reinterpret_cast<DWORD_PTR>(pBaseRelocDesc) + sizeof(IMAGE_BASE_RELOCATION));
				std::vector<PERELOCDATA> vecRelocData;
				for (auto i = 0UL; i < dwNumRelocEntries; ++i, ++pwRelocEntry) {
					if (!IsPtrSafe(pwRelocEntry))
						break;
					//Getting HIGH 4 bits of reloc's entry WORD —> reloc type.
					const WORD wRelocType = (*pwRelocEntry & 0xF000) >> 12;
					vecRelocData.emplace_back(PtrToOffset(pwRelocEntry), wRelocType, static_cast<WORD>((*pwRelocEntry) & 0x0fff)/*Low 12 bits —> Offset*/);
					if (wRelocType == IMAGE_REL_BASED_HIGHADJ) {	//The base relocation adds the high 16 bits of the difference to the 16-bit field at offset.
						//The 16-bit field represents the high value of a 32-bit word. 
						//The low 16 bits of the 32-bit value are stored in the 16-bit word that follows this base relocation.
						//This means that this base relocation occupies two slots. (MSDN)
						if (!IsPtrSafe(++pwRelocEntry)) {
							vecRelocData.clear();
							break;
						}

						vecRelocData.emplace_back(PtrToOffset(pwRelocEntry), wRelocType, *pwRelocEntry /*The low 16-bit field.*/);
						--dwNumRelocEntries; //to compensate ++pwRelocEntry.
					}
				}

				vecRelocs.emplace_back(PtrToOffset(pBaseRelocDesc), *pBaseRelocDesc, std::move(vecRelocData));

				//Too big (bogus) SizeOfBlock may cause DWORD_PTR overflow. Checking to prevent.
				if (IsSumOverflow(reinterpret_cast<DWORD_PTR>(pBaseRelocDesc), static_cast<DWORD_PTR>(pBaseRelocDesc->SizeOfBlock)))
					break;

				pBaseRelocDesc = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<DWORD_PTR>(pBaseRelocDesc)
					+ static_cast<DWORD_PTR>(pBaseRelocDesc->SizeOfBlock));
				if (!IsPtrSafe(pBaseRelocDesc))
					break;
			}

			return vecRelocs.empty() ? std::nullopt : std::optional<PERELOC_VEC>(std::move(vecRelocs));
		}
		catch (const std::bad_alloc&) {
			vecRelocs.clear();
			MessageBoxW(nullptr, L"E_OUTOFMEMORY error while trying to get Relocation table.\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}
		catch (...) {
			MessageBoxW(nullptr, L"Unknown exception raised while trying to get Relocation table.\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}

		return std::nullopt;
	}

	auto Clibpe::GetDebug()const->std::optional<PEDEBUG_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		const auto dwDebugDirRVA = GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_DEBUG);
		if (!dwDebugDirRVA)
			return std::nullopt;

		PIMAGE_DEBUG_DIRECTORY pDebugDir;
		DWORD dwDebugDirSize;
		PIMAGE_SECTION_HEADER pDebugSecHdr = GetSecHdrFromName(".debug");
		if (pDebugSecHdr && (pDebugSecHdr->VirtualAddress == dwDebugDirRVA)) {
			pDebugDir = reinterpret_cast<PIMAGE_DEBUG_DIRECTORY>(GetBaseAddr() + static_cast<DWORD_PTR>(pDebugSecHdr->PointerToRawData));
			dwDebugDirSize = GetDirEntrySize(IMAGE_DIRECTORY_ENTRY_DEBUG) * static_cast<DWORD>(sizeof(IMAGE_DEBUG_DIRECTORY));
		}
		else { //Looking for the debug directory.
			if (pDebugSecHdr = GetSecHdrFromRVA(dwDebugDirRVA); pDebugSecHdr == nullptr)
				return std::nullopt;

			if (pDebugDir = static_cast<PIMAGE_DEBUG_DIRECTORY>(RVAToPtr(dwDebugDirRVA)); pDebugDir == nullptr)
				return std::nullopt;

			dwDebugDirSize = GetDirEntrySize(IMAGE_DIRECTORY_ENTRY_DEBUG);
		}

		const auto dwDebugEntries = dwDebugDirSize / static_cast<DWORD>(sizeof(IMAGE_DEBUG_DIRECTORY));

		if (!dwDebugEntries || IsSumOverflow(reinterpret_cast<DWORD_PTR>(pDebugDir), static_cast<DWORD_PTR>(dwDebugDirSize)) ||
			!IsPtrSafe(reinterpret_cast<DWORD_PTR>(pDebugDir) + static_cast<DWORD_PTR>(dwDebugDirSize)))
			return std::nullopt;

		PEDEBUG_VEC vecDebug;
		try {
			for (auto i = 0UL; i < dwDebugEntries; ++i) {
				PEDEBUGDBGHDR stDbgHdr;
				for (auto iterDbgHdr = 0UL; iterDbgHdr < (sizeof(PEDEBUGDBGHDR::dwHdr) / sizeof(DWORD)); iterDbgHdr++) {
					stDbgHdr.dwHdr[iterDbgHdr] = GetTData<DWORD>(static_cast<size_t>(pDebugDir->PointerToRawData) + (sizeof(DWORD) * iterDbgHdr));
				}

				if (pDebugDir->Type == IMAGE_DEBUG_TYPE_CODEVIEW) {
					DWORD dwOffset = 0;
					if (stDbgHdr.dwHdr[0] == 0x53445352) { //"RSDS"
						dwOffset = sizeof(DWORD) * 6;
					}
					else if (stDbgHdr.dwHdr[0] == 0x3031424E) { //"NB10"
						dwOffset = sizeof(DWORD) * 4;
					}

					std::string strPDBName;
					if (dwOffset > 0) {
						for (unsigned iterStr = 0; iterStr < MAX_PATH; iterStr++) {
							const auto byte = GetTData<BYTE>(pDebugDir->PointerToRawData + dwOffset + iterStr);
							if (byte == 0) //End of string.
								break;
							strPDBName += byte;
						}
					}
					stDbgHdr.strPDBName = std::move(strPDBName);
				}

				vecDebug.emplace_back(PtrToOffset(pDebugDir), *pDebugDir, stDbgHdr);
				if (!IsPtrSafe(++pDebugDir))
					break;
			}

			return vecDebug.empty() ? std::nullopt : std::optional<PEDEBUG_VEC>(std::move(vecDebug));
		}
		catch (const std::bad_alloc&) {
			vecDebug.clear();
			MessageBoxW(nullptr, L"E_OUTOFMEMORY error while trying to get Debug info.\r\n"
				L"File seems to be corrupted.", L"Error", MB_ICONERROR);
		}

		return std::nullopt;
	}

	auto Clibpe::GetTLS()const->std::optional<PETLS>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		const auto dwTLSDirRVA = GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_TLS);
		if (!dwTLSDirRVA)
			return std::nullopt;

		std::vector<DWORD> vecTLSCallbacks;
		try {
			ULONGLONG ullAddressOfCallBacks { };
			PETLS::UNPETLS varTLSDir;
			PDWORD pdwTLSPtr;

			if (m_enPEType == EFileType::PE32) {
				const auto pTLSDir32 = static_cast<PIMAGE_TLS_DIRECTORY32>(RVAToPtr(dwTLSDirRVA));
				if (pTLSDir32 == nullptr)
					return std::nullopt;

				varTLSDir.stTLSDir32 = *pTLSDir32;
				pdwTLSPtr = reinterpret_cast<PDWORD>(pTLSDir32);
				ullAddressOfCallBacks = pTLSDir32->AddressOfCallBacks;
			}
			else if (m_enPEType == EFileType::PE64) {
				const auto pTLSDir64 = static_cast<PIMAGE_TLS_DIRECTORY64>(RVAToPtr(dwTLSDirRVA));
				if (pTLSDir64 == nullptr)
					return std::nullopt;

				varTLSDir.stTLSDir64 = *pTLSDir64;
				pdwTLSPtr = reinterpret_cast<PDWORD>(pTLSDir64);
				ullAddressOfCallBacks = pTLSDir64->AddressOfCallBacks;
			}
			else
				return std::nullopt;

			if (auto pTLSCallbacks = static_cast<PDWORD>(RVAToPtr(ullAddressOfCallBacks - GetImageBase())); pTLSCallbacks != nullptr) {
				while (*pTLSCallbacks) {
					vecTLSCallbacks.push_back(*pTLSCallbacks);
					if (!IsPtrSafe(++pTLSCallbacks)) {
						vecTLSCallbacks.clear();
						break;
					}
				}
			}

			return std::make_optional<PETLS>(PtrToOffset(pdwTLSPtr), varTLSDir, std::move(vecTLSCallbacks));
		}
		catch (const std::bad_alloc&) {
			vecTLSCallbacks.clear();
			MessageBoxW(nullptr, L"E_OUTOFMEMORY error while trying to get TLS table.\r\n"
				L"File seems to be corrupted.", L"Error", MB_ICONERROR);
		}
		catch (...) {
			MessageBoxW(nullptr, L"Unknown exception raised while trying to get TLS table.\r\nFile seems to be corrupted.",
				L"Error", MB_ICONERROR);
		}

		return std::nullopt;
	}

	auto Clibpe::GetLoadConfig()const->std::optional<PELOADCONFIG>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		switch (m_enPEType) {
		case EFileType::PE32:
		{
			const auto pLCD32 = static_cast<PIMAGE_LOAD_CONFIG_DIRECTORY32>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG)));
			if (!pLCD32 || !IsPtrSafe(reinterpret_cast<DWORD_PTR>(pLCD32) + sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32)))
				return std::nullopt;

			PELOADCONFIG stLCD32 { PtrToOffset(pLCD32) };
			stLCD32.unLCD.stLCD32 = *pLCD32;

			return std::optional<PELOADCONFIG>(std::move(stLCD32));
		}
		case EFileType::PE64:
		{
			const auto pLCD64 = static_cast<PIMAGE_LOAD_CONFIG_DIRECTORY64>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG)));
			if (!pLCD64 || !IsPtrSafe(reinterpret_cast<DWORD_PTR>(pLCD64) + sizeof(PIMAGE_LOAD_CONFIG_DIRECTORY64)))
				return std::nullopt;

			PELOADCONFIG stLCD64 { PtrToOffset(pLCD64) };
			stLCD64.unLCD.stLCD64 = *pLCD64;

			return std::optional<PELOADCONFIG>(std::move(stLCD64));
		}
		default:
			return std::nullopt;
		}
	}

	auto Clibpe::GetBoundImport()const->std::optional<PEBOUNDIMPORT_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		auto pBoundImpDesc = static_cast<PIMAGE_BOUND_IMPORT_DESCRIPTOR>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT)));
		if (pBoundImpDesc == nullptr)
			return std::nullopt;

		PEBOUNDIMPORT_VEC vecBoundImp;
		while (pBoundImpDesc->TimeDateStamp) {
			std::string strModuleName;
			std::vector<PEBOUNDFORWARDER> vecBoundForwarders;

			auto pBoundImpForwarder = reinterpret_cast<PIMAGE_BOUND_FORWARDER_REF>(pBoundImpDesc + 1);
			if (!IsPtrSafe(pBoundImpForwarder))
				break;

			for (unsigned i = 0; i < pBoundImpDesc->NumberOfModuleForwarderRefs; ++i) {
				std::string strForwarderModuleName { };

				if (const auto szName = reinterpret_cast<LPCSTR>(reinterpret_cast<DWORD_PTR>(pBoundImpDesc) + pBoundImpForwarder->OffsetModuleName);
					IsPtrSafe(szName)) {
					if (szName && (StringCchLengthA(szName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
						strForwarderModuleName = szName;
					}
				}

				vecBoundForwarders.emplace_back(PtrToOffset(pBoundImpForwarder), *pBoundImpForwarder, std::move(strForwarderModuleName));

				if (!IsPtrSafe(++pBoundImpForwarder))
					break;

				pBoundImpDesc = reinterpret_cast<PIMAGE_BOUND_IMPORT_DESCRIPTOR>(reinterpret_cast<DWORD_PTR>(pBoundImpDesc) + sizeof(IMAGE_BOUND_FORWARDER_REF));
				if (!IsPtrSafe(pBoundImpDesc))
					break;
			}

			if (const auto szName = reinterpret_cast<LPCSTR>(reinterpret_cast<DWORD_PTR>(pBoundImpDesc) + pBoundImpDesc->OffsetModuleName);
				IsPtrSafe(szName)) {
				if (StringCchLengthA(szName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER) {
					strModuleName = szName;
				}
			}

			vecBoundImp.emplace_back(PtrToOffset(pBoundImpDesc), *pBoundImpDesc, std::move(strModuleName), std::move(vecBoundForwarders));

			if (!IsPtrSafe(++pBoundImpDesc))
				break;
		}

		return vecBoundImp.empty() ? std::nullopt : std::optional<PEBOUNDIMPORT_VEC>(std::move(vecBoundImp));
	}

	auto Clibpe::GetDelayImport()const->std::optional<PEDELAYIMPORT_VEC>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		auto pDelayImpDescr = static_cast<PIMAGE_DELAYLOAD_DESCRIPTOR>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT)));
		if (pDelayImpDescr == nullptr)
			return std::nullopt;

		PEDELAYIMPORT_VEC vecDelayImp;

		if (m_enPEType == EFileType::PE32) {
			while (pDelayImpDescr->DllNameRVA) {
				auto pThunk32Name = reinterpret_cast<PIMAGE_THUNK_DATA32>(static_cast<DWORD_PTR>(pDelayImpDescr->ImportNameTableRVA));
				if (pThunk32Name == nullptr) {
					if (!IsPtrSafe(++pDelayImpDescr))
						break;
				}
				else {
					std::vector<PEDELAYIMPORTFUNC> vecFunc;
					pThunk32Name = static_cast<PIMAGE_THUNK_DATA32>(RVAToPtr(reinterpret_cast<DWORD_PTR>(pThunk32Name)));
					auto pThunk32IAT = static_cast<PIMAGE_THUNK_DATA32>(RVAToPtr(pDelayImpDescr->ImportAddressTableRVA));
					auto pThunk32BoundIAT = static_cast<PIMAGE_THUNK_DATA32>(RVAToPtr(pDelayImpDescr->BoundImportAddressTableRVA));
					auto pThunk32UnloadInfoTable = static_cast<PIMAGE_THUNK_DATA32>(RVAToPtr(pDelayImpDescr->UnloadInformationTableRVA));

					if (pThunk32Name == nullptr)
						break;

					while (pThunk32Name->u1.AddressOfData) {
						PEDELAYIMPORTFUNC::UNPEDELAYIMPORTTHUNK unDelayImpThunk32 { };
						unDelayImpThunk32.st32.stImportAddressTable = *pThunk32Name;
						unDelayImpThunk32.st32.stImportNameTable = pThunk32IAT ? *pThunk32IAT : IMAGE_THUNK_DATA32 { };
						unDelayImpThunk32.st32.stBoundImportAddressTable = pThunk32BoundIAT ? *pThunk32BoundIAT : IMAGE_THUNK_DATA32 { };
						unDelayImpThunk32.st32.stUnloadInformationTable = pThunk32UnloadInfoTable ? *pThunk32UnloadInfoTable : IMAGE_THUNK_DATA32 { };

						std::string strFuncName;
						IMAGE_IMPORT_BY_NAME stImpByName { };
						if (!(pThunk32Name->u1.Ordinal & IMAGE_ORDINAL_FLAG32)) {
							if (const auto pName = static_cast<PIMAGE_IMPORT_BY_NAME>(RVAToPtr(pThunk32Name->u1.AddressOfData));
								pName && (StringCchLengthA(pName->Name, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
								stImpByName = *pName;
								strFuncName = pName->Name;
							}
						}
						vecFunc.emplace_back(unDelayImpThunk32, stImpByName, std::move(strFuncName));

						if (!IsPtrSafe(++pThunk32Name))
							break;
						if (pThunk32IAT) {
							if (!IsPtrSafe(++pThunk32IAT))
								break;
						}
						if (pThunk32BoundIAT) {
							if (!IsPtrSafe(++pThunk32BoundIAT))
								break;
						}
						if (pThunk32UnloadInfoTable) {
							if (!IsPtrSafe(++pThunk32UnloadInfoTable))
								break;
						}
					}

					std::string strDllName;
					if (const auto szName = static_cast<LPCSTR>(RVAToPtr(pDelayImpDescr->DllNameRVA));
						szName != nullptr && (StringCchLengthA(szName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
						strDllName = szName;
					}
					vecDelayImp.emplace_back(PtrToOffset(pDelayImpDescr), *pDelayImpDescr, std::move(strDllName), std::move(vecFunc));

					if (!IsPtrSafe(++pDelayImpDescr))
						break;
				}
			}
		}
		else if (m_enPEType == EFileType::PE64) {
			while (pDelayImpDescr->DllNameRVA) {
				auto pThunk64Name = reinterpret_cast<PIMAGE_THUNK_DATA64>(static_cast<DWORD_PTR>(pDelayImpDescr->ImportNameTableRVA));

				if (pThunk64Name == nullptr) {
					if (!IsPtrSafe(++pDelayImpDescr))
						break;
				}
				else {
					std::vector<PEDELAYIMPORTFUNC> vecFunc;
					pThunk64Name = static_cast<PIMAGE_THUNK_DATA64>(RVAToPtr(reinterpret_cast<DWORD_PTR>(pThunk64Name)));
					auto pThunk64IAT = static_cast<PIMAGE_THUNK_DATA64>(RVAToPtr(pDelayImpDescr->ImportAddressTableRVA));
					auto pThunk64BoundIAT = static_cast<PIMAGE_THUNK_DATA64>(RVAToPtr(pDelayImpDescr->BoundImportAddressTableRVA));
					auto pThunk64UnloadInfoTable = static_cast<PIMAGE_THUNK_DATA64>(RVAToPtr(pDelayImpDescr->UnloadInformationTableRVA));

					if (pThunk64Name == nullptr)
						break;

					while (pThunk64Name->u1.AddressOfData) {
						PEDELAYIMPORTFUNC::UNPEDELAYIMPORTTHUNK unDelayImpThunk64 { };
						unDelayImpThunk64.st64.stImportAddressTable = *pThunk64Name;
						unDelayImpThunk64.st64.stImportNameTable = pThunk64IAT ? *pThunk64IAT : IMAGE_THUNK_DATA64 { };
						unDelayImpThunk64.st64.stBoundImportAddressTable = pThunk64BoundIAT ? *pThunk64BoundIAT : IMAGE_THUNK_DATA64 { };
						unDelayImpThunk64.st64.stUnloadInformationTable = pThunk64UnloadInfoTable ? *pThunk64UnloadInfoTable : IMAGE_THUNK_DATA64 { };

						std::string strFuncName;
						IMAGE_IMPORT_BY_NAME stImpByName { };
						if (!(pThunk64Name->u1.Ordinal & IMAGE_ORDINAL_FLAG64)) {
							if (const auto pName = static_cast<PIMAGE_IMPORT_BY_NAME>(RVAToPtr(pThunk64Name->u1.AddressOfData));
								pName && (StringCchLengthA(pName->Name, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
								stImpByName = *pName;
								strFuncName = pName->Name;
							}
						}
						vecFunc.emplace_back(unDelayImpThunk64, stImpByName, std::move(strFuncName));

						if (!IsPtrSafe(++pThunk64Name))
							break;
						if (pThunk64IAT) {
							if (!IsPtrSafe(++pThunk64IAT))
								break;
						}
						if (pThunk64BoundIAT) {
							if (!IsPtrSafe(++pThunk64BoundIAT))
								break;
						}
						if (pThunk64UnloadInfoTable) {
							if (!IsPtrSafe(++pThunk64UnloadInfoTable))
								break;
						}
					}

					std::string strDllName;
					if (const auto szName = static_cast<LPCSTR>(RVAToPtr(pDelayImpDescr->DllNameRVA));
						szName != nullptr && (StringCchLengthA(szName, MAX_PATH, nullptr) != STRSAFE_E_INVALID_PARAMETER)) {
						strDllName = szName;
					}
					vecDelayImp.emplace_back(PtrToOffset(pDelayImpDescr), *pDelayImpDescr, std::move(strDllName), std::move(vecFunc));

					if (!IsPtrSafe(++pDelayImpDescr))
						break;
				}
			}
		}

		return vecDelayImp.empty() ? std::nullopt : std::optional<PEDELAYIMPORT_VEC>(std::move(vecDelayImp));
	}

	auto Clibpe::GetCOMDescriptor()const->std::optional<PECOMDESCRIPTOR>
	{
		assert(m_fOpened);
		if (!m_fOpened)
			return std::nullopt;

		const auto pCOR20Hdr = static_cast<PIMAGE_COR20_HEADER>(RVAToPtr(GetDirEntryRVA(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR)));
		if (pCOR20Hdr == nullptr)
			return std::nullopt;

		return std::make_optional<PECOMDESCRIPTOR>(PtrToOffset(pCOR20Hdr), *pCOR20Hdr);
	}


	//Private methods.

	auto Clibpe::GetFileSize()const->ULONGLONG
	{
		return m_spnData.size();
	}

	auto Clibpe::GetBaseAddr()const->DWORD_PTR
	{
		return reinterpret_cast<DWORD_PTR>(m_spnData.data());
	}

	auto Clibpe::GetDosPtr()const->const IMAGE_DOS_HEADER*
	{
		return reinterpret_cast<const IMAGE_DOS_HEADER*>(m_spnData.data());
	}

	auto Clibpe::GetDirEntryRVA(DWORD dwEntry)const->DWORD
	{
		if (!m_fHasNTHdr)
			return { };

		switch (m_enPEType) {
		case EFileType::PE32:
			return m_pNTHeader32->OptionalHeader.DataDirectory[dwEntry].VirtualAddress;
		case EFileType::PE64:
			return m_pNTHeader64->OptionalHeader.DataDirectory[dwEntry].VirtualAddress;
		default:
			return { };
		}
	}

	auto Clibpe::GetDirEntrySize(DWORD dwEntry)const->DWORD
	{
		if (!m_fHasNTHdr)
			return { };

		switch (m_enPEType) {
		case EFileType::PE32:
			return m_pNTHeader32->OptionalHeader.DataDirectory[dwEntry].Size;
		case EFileType::PE64:
			return m_pNTHeader64->OptionalHeader.DataDirectory[dwEntry].Size;
		default:
			return { };
		}

	}

	auto Clibpe::GetImageBase()const->ULONGLONG
	{
		switch (m_enPEType) {
		case EFileType::PE32:
			return m_pNTHeader32->OptionalHeader.ImageBase;
		case EFileType::PE64:
			return m_pNTHeader64->OptionalHeader.ImageBase;
		default:
			return { };
		}
	}

	auto Clibpe::GetSecHdrFromName(LPCSTR lpszName)const->PIMAGE_SECTION_HEADER
	{
		if (!m_fHasNTHdr)
			return nullptr;

		PIMAGE_SECTION_HEADER pSecHdr;
		WORD wNumberOfSections;

		switch (m_enPEType) {
		case EFileType::PE32:
			pSecHdr = IMAGE_FIRST_SECTION(m_pNTHeader32);
			wNumberOfSections = m_pNTHeader32->FileHeader.NumberOfSections;
			break;
		case EFileType::PE64:
			pSecHdr = IMAGE_FIRST_SECTION(m_pNTHeader64);
			wNumberOfSections = m_pNTHeader64->FileHeader.NumberOfSections;
			break;
		default:
			return nullptr;
		}

		for (auto i = 0UL; i < wNumberOfSections; ++i, ++pSecHdr) {
			if (!IsPtrSafe(reinterpret_cast<DWORD_PTR>(pSecHdr) + sizeof(IMAGE_SECTION_HEADER)))
				break;
			if (strncmp(reinterpret_cast<char*>(pSecHdr->Name), lpszName, IMAGE_SIZEOF_SHORT_NAME) == 0)
				return pSecHdr;
		}

		return nullptr;
	}

	auto Clibpe::GetSecHdrFromRVA(ULONGLONG ullRVA)const->PIMAGE_SECTION_HEADER
	{
		if (!m_fHasNTHdr)
			return nullptr;

		PIMAGE_SECTION_HEADER pSecHdr;
		WORD wNumberOfSections;

		switch (m_enPEType) {
		case EFileType::PE32:
			pSecHdr = IMAGE_FIRST_SECTION(m_pNTHeader32);
			wNumberOfSections = m_pNTHeader32->FileHeader.NumberOfSections;
			break;
		case EFileType::PE64:
			pSecHdr = IMAGE_FIRST_SECTION(m_pNTHeader64);
			wNumberOfSections = m_pNTHeader64->FileHeader.NumberOfSections;
			break;
		default:
			return nullptr;
		}


		for (auto i = 0UL; i < wNumberOfSections; ++i, ++pSecHdr) {
			if (!IsPtrSafe(reinterpret_cast<DWORD_PTR>(pSecHdr) + sizeof(IMAGE_SECTION_HEADER)))
				return nullptr;

			//Is RVA within this section?
			if ((ullRVA >= pSecHdr->VirtualAddress) && (ullRVA < (pSecHdr->VirtualAddress + pSecHdr->Misc.VirtualSize))) {
				return pSecHdr;
			}
		}

		return nullptr;
	}

	template<typename T>
	auto Clibpe::GetTData(ULONGLONG ullOffset)const->T
	{
		if (ullOffset > (GetFileSize() - sizeof(T))) //Check for file size exceeding.
			return { };

		return *reinterpret_cast<T*>(GetBaseAddr() + ullOffset);
	}

	template<typename T>
	auto Clibpe::IsPtrSafe(const T tAddr, bool fCanReferenceBoundary)const->bool
	{
		/**************************************************************************************************
		* This func checks given pointer for nullptr and, more important, whether it fits allowed bounds. *
		* In PE headers there are plenty of places where wrong (bogus) values for pointers might reside,  *
		* causing many runtime «fun» if trying to dereference them.                                       *
		* Second arg (fCanReferenceBoundary) shows if pointer can point to the very end of a file, it's   *
		* valid for some PE structures. Template is used just for convenience, sometimes there is a need  *
		* to check pure address DWORD_PTR instead of a pointer.                                           *
		**************************************************************************************************/
		DWORD_PTR dwAddr;
		if constexpr (!std::is_same_v<T, DWORD_PTR>) {
			dwAddr = reinterpret_cast<DWORD_PTR>(tAddr);
		}
		else {
			dwAddr = tAddr;
		}

		const auto ullBase = GetBaseAddr();
		const auto ullMaxAddr = ullBase + GetFileSize();

		return ((dwAddr == 0) || (dwAddr < ullBase)) ? false :
			(fCanReferenceBoundary ? dwAddr <= ullMaxAddr : dwAddr < ullMaxAddr);
	}

	auto Clibpe::PtrToOffset(LPCVOID lp)const->DWORD
	{
		if (lp == nullptr)
			return 0;

		return static_cast<DWORD>(reinterpret_cast<DWORD_PTR>(lp) - GetBaseAddr());
	}

	auto Clibpe::RVAToPtr(ULONGLONG ullRVA)const->LPVOID
	{
		const auto pSecHdr = GetSecHdrFromRVA(ullRVA);
		if (pSecHdr == nullptr)
			return nullptr;

		const auto ptr = reinterpret_cast<LPVOID>(GetBaseAddr() + ullRVA
			- static_cast<DWORD_PTR>(pSecHdr->VirtualAddress) + static_cast<DWORD_PTR>(pSecHdr->PointerToRawData));

		return IsPtrSafe(ptr, true) ? ptr : nullptr;
	}

	bool Clibpe::ParseDOSHeader()
	{
		//If a file has at least MSDOS header signature then we can assume
		//that this is a minimally correct PE file, and process further.
		return GetDosPtr()->e_magic == IMAGE_DOS_SIGNATURE;
	}

	bool Clibpe::ParseNTFileOptHeader()
	{
		const auto pNTHeader = reinterpret_cast<PIMAGE_NT_HEADERS32>(GetBaseAddr()
			+ static_cast<DWORD_PTR>(GetDosPtr()->e_lfanew));
		if (!IsPtrSafe(reinterpret_cast<DWORD_PTR>(pNTHeader) + sizeof(IMAGE_NT_HEADERS32)))
			return false;

		if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
			return false;

		switch (pNTHeader->OptionalHeader.Magic) {
		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
			m_enPEType = EFileType::PE32;
			m_pNTHeader32 = pNTHeader;
			break;
		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
			m_enPEType = EFileType::PE64;
			m_pNTHeader64 = reinterpret_cast<PIMAGE_NT_HEADERS64>(pNTHeader);
			break;
			//case IMAGE_ROM_OPTIONAL_HDR_MAGIC: //Not implemented.
		default:
			return false;
		}

		m_fHasNTHdr = true;

		return true;
	}

	//Helper methods.

	export [[nodiscard]] inline constexpr auto GetFileType(const PENTHDR& stNTHdr) -> EFileType
	{
		const auto& refNTHdr = stNTHdr.unHdr.stNTHdr32;
		if (refNTHdr.Signature != IMAGE_NT_SIGNATURE)
			return EFileType::UNKNOWN;

		const auto& refOptHdr = refNTHdr.OptionalHeader;
		switch (refOptHdr.Magic) {
		case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
			return EFileType::PE32;
		case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
			return EFileType::PE64;
		case IMAGE_ROM_OPTIONAL_HDR_MAGIC:
			return EFileType::PEROM;
		default:
			return EFileType::UNKNOWN;
		}
	}

	export [[nodiscard]] inline constexpr auto GetImageBase(const PENTHDR& stNTHdr) -> ULONGLONG
	{
		switch (GetFileType(stNTHdr)) {
		case EFileType::PE32:
			return stNTHdr.unHdr.stNTHdr32.OptionalHeader.ImageBase;
		case EFileType::PE64:
			return stNTHdr.unHdr.stNTHdr64.OptionalHeader.ImageBase;
		default:
			return { };
		}
	}

	export [[nodiscard]] inline constexpr auto GetOffsetFromRVA(ULONGLONG ullRVA, const PESECHDR_VEC& vecSecHdr) -> DWORD
	{
		for (const auto& stSec : vecSecHdr) {
			if (const auto pSecHdr = &stSec.stSecHdr; (ullRVA >= pSecHdr->VirtualAddress) //Is RVA within this section?
				&& (ullRVA < (pSecHdr->VirtualAddress + pSecHdr->Misc.VirtualSize))) {
				return static_cast<DWORD>(ullRVA) - pSecHdr->VirtualAddress + pSecHdr->PointerToRawData;
			}
		}

		return { };
	}

	export [[nodiscard]] inline constexpr auto FlatResources(const PERESROOT& stResRoot) -> PERESFLAT_VEC
	{
		std::size_t sTotalRes { 0 }; //How many resources total?
		for (const auto& iterRoot : stResRoot.vecResData) { //To reserve space in vector, count total amount of resources.
			const auto pResDirEntry = &iterRoot.stResDirEntry; //Level Root
			if (pResDirEntry->DataIsDirectory) {
				const auto pstResLvL2 = &iterRoot.stResLvL2;
				for (const auto& iterLvL2 : pstResLvL2->vecResData) {
					const auto pResDirEntry2 = &iterLvL2.stResDirEntry; //Level 2 IMAGE_RESOURCE_DIRECTORY_ENTRY
					if (pResDirEntry2->DataIsDirectory) {
						sTotalRes += iterLvL2.stResLvL3.vecResData.size(); //Level 3
					}
					else {
						++sTotalRes;
					}
				}
			}
			else {
				++sTotalRes;
			}
		}

		std::vector<PERESFLAT> vecData;
		vecData.reserve(sTotalRes);
		for (auto& iterRoot : stResRoot.vecResData) {
			PERESFLAT stRes { };
			const auto pResDirEntryRoot = &iterRoot.stResDirEntry; //Level Root IMAGE_RESOURCE_DIRECTORY_ENTRY
			if (pResDirEntryRoot->NameIsString) {
				stRes.wsvTypeStr = iterRoot.wstrResName;
			}
			else {
				stRes.wTypeID = pResDirEntryRoot->Id;
			}

			if (pResDirEntryRoot->DataIsDirectory) {
				for (auto& iterLvL2 : iterRoot.stResLvL2.vecResData) {
					const auto pResDirEntry2 = &iterLvL2.stResDirEntry; //Level 2 IMAGE_RESOURCE_DIRECTORY_ENTRY
					if (pResDirEntry2->NameIsString) {
						stRes.wsvNameStr = iterLvL2.wstrResName;
					}
					else {
						stRes.wNameID = pResDirEntry2->Id;
					}

					if (pResDirEntry2->DataIsDirectory) {
						for (auto& iterLvL3 : iterLvL2.stResLvL3.vecResData) {
							const auto pResDirEntry3 = &iterLvL3.stResDirEntry; //Level 3 IMAGE_RESOURCE_DIRECTORY_ENTRY
							if (pResDirEntry3->NameIsString) {
								stRes.wsvLangStr = iterLvL3.wstrResName;
							}
							else {
								stRes.wLangID = pResDirEntry3->Id;
							}

							stRes.spnData = iterLvL3.vecRawResData;
							vecData.emplace_back(stRes);
						}
					}
					else {
						stRes.spnData = iterLvL2.vecRawResData;
						vecData.emplace_back(stRes);
					}
				}
			}
			else {
				stRes.spnData = iterRoot.vecRawResData;
				vecData.emplace_back(stRes);
			}
		}

		return vecData;
	}
}