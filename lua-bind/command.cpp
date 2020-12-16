#include "str2args.h"
#include "command.h"
#include "Common/StdOutStream.h"
#include "Common/UTFConvert.h"


#include "7zip/UI/Common/ArchiveCommandLine.h"
#include "CPP/Common/StringConvert.h"
//#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "LuaIntf.h"

extern UString g_LastErr;

extern "C"
{
    extern int p7zip_executeCommand(const char *cmd) {
        int argc = 0;
        char temp[ARGC_MAX][ARGV_LEN_MAX];
        char *argv[ARGC_MAX];
        if (!str2args(cmd, temp, &argc)) {
            return 7;
        }
        for (int i = 0; i < argc; i++) {
            argv[i] = temp[i];
                //        printf("arg[%d]:[%s]", i, argv[i]);
        }
        int ret = p7zip_main(argc, argv);
        return ret;
    }


    int execute(lua_State *L) {
        const char *cmd = luaL_checkstring( L,1 );
        int ret = p7zip_executeCommand(cmd);
        lua_pushnumber(L, ret);
        AString dis;
        ConvertUnicodeToUTF8(g_LastErr, dis);
        lua_pushstring(L, dis.Ptr());
        return 2;
    }
 
    extern "C" int luaopen_p7zip(lua_State *L)
    {
        LuaIntf::LuaRef mod = LuaIntf::LuaRef::createTable(L);
        //      UString GetUnicodeString(const AString &ansiString)
        typedef UString (*a2us)(const AString& ansiString);
        typedef AString (*u2as)(const UString& ansiString);

		LuaIntf::LuaBinding lb = LuaIntf::LuaBinding(mod);
        lb.beginModule("p7zip")
		.addFunction("exec",    &p7zip_executeCommand, LUA_ARGS(const char*))
		.addFunction("execute", &p7zip_executeCommand, LUA_ARGS(const char*))
		.addFunction("ConvertUnicodeToUTF8", &ConvertUnicodeToUTF8, LUA_ARGS(const UString &src, AString &dis))
		.addFunction("A2UString", [](AString& as) -> UString{return GetUnicodeString(as);})
		.addFunction("U2AString", [](UString& s) ->  AString{return GetAnsiString(s);})
		.addFunction("GetUnicodeString", (a2us)&GetUnicodeString, LUA_ARGS(const AString& ansiString))
		.addFunction("GetAnsiString",    (u2as)&GetAnsiString,    LUA_ARGS(const UString& unicodeString))
//		.beginModule("g")
			.addVariableRef("StdOut", &g_StdOut)
			.addVariableRef("StdErr", &g_StdErr)
		.endModule()
        .beginClass<AString>("AString")
            .addConstructor(LUA_SP(AString), LUA_ARGS(void))
            .addConstructor(LUA_SP(AString), LUA_ARGS(const char *))
            .addFunction("IsEqualTo", &AString::IsEqualTo, LUA_ARGS(const char *))
            .addFunction("Trim", &AString::Trim)
//            .addStaticFunction("tostring1", [](AString& self)->const char*{return self.Ptr();})
            .addMetaFunction("__tostring", [](AString& self)->const char*{return self.Ptr();})
			.addMetaFunction("__add", [](AString& self, AString& second)->AString{return self+second;})
        .endClass()
        .beginClass<UString>("UString")
            .addConstructor(LUA_SP(UString), LUA_ARGS(void))
            .addMetaFunction("tostring", [L](UString& self)->const char*{return GetAnsiString(self).Ptr();})
        .endClass()
//        .beginClass<CArcCmdLineOptions>("CArcCmdLineOptions")
//            .addVariableRef("HelpMode", &CArcCmdLineOptions::HelpMode)
//        .endClass()
//        .beginClass<CArcCmdLineParser>("CArcCmdLineParser")
//            .addFunction("Parse1", &CArcCmdLineParser::Parse1, LUA_ARGS(const UStringVector, CArcCmdLineOptions))
//            .addFunction("Parse2", &CArcCmdLineParser::Parse2, LUA_ARGS(CArcCmdLineOptions))
//            .addStaticFunction("lambda", [] {
//                // you can use C++11 lambda expression here too
//                return "yes";
//            })
//        .endClass()
        .beginClass<CStdOutStream>("CStdOutStream")
            .addConstructor(LUA_SP(std::shared_ptr<CStdOutStream>), LUA_ARGS(FILE *))
            .addFunction("Open", &CStdOutStream::Open, LUA_ARGS(const char *fileName, const char * mode))
            .addFunction("Close", &CStdOutStream::Close)
            .addFunction("Print", &CStdOutStream::Print, LUA_ARGS(const char*))
            .addFunction("Flush", &CStdOutStream::Flush)
        .endClass()
        .beginClass<COpenOptions>("COpenOptions")
            .addConstructor(LUA_SP(std::shared_ptr<COpenOptions>), LUA_ARGS(void))
            .addVariable("filePath", &COpenOptions::filePath)
            .addVariable("codecs", &COpenOptions::codecs)
            .addVariable("openType", &COpenOptions::openType)
            .addVariable("types", &COpenOptions::types)
            .addVariable("excludedFormats", &COpenOptions::excludedFormats)
            .addVariable("stream", &COpenOptions::stream)
            .addVariable("seqStream", &COpenOptions::seqStream)
            .addVariable("callback", &COpenOptions::callback)
            .addVariable("filePath", &COpenOptions::filePath)
            .addVariable("stdInMode", &COpenOptions::stdInMode)
        .endClass()
        .beginClass<CArc>("CArc")
            .addConstructor(LUA_SP(std::shared_ptr<CArc>), LUA_ARGS(void))
            .addVariable("Path", &CArc::Path)
            .addVariable("filePath", &CArc::filePath)
            .addVariable("Archive", &CArc::Archive)
            .addVariable("InStream", &CArc::InStream)
            .addFunction("OpenStreamOrFile", &CArc::OpenStreamOrFile, LUA_ARGS(COpenOptions&op))
//            .addMetaFunction("OpenStreamOrFile", [L](CArc& self, COpenOptions& op){
//                printf("op:%x", &op);
//                lua_pushlightuserdata(L, &op);
//                return self.OpenStreamOrFile(op);
//            })
            .addFunction("Close", &CArc::Close)
            .addFunction("GetItem", &CArc::GetItem, LUA_ARGS(UInt32 index, CReadArcItem &item))
            .addFunction("GetItemSize", &CArc::GetItemSize, LUA_ARGS(UInt32 index, UInt64 &size, bool &defined))
            .addFunction("OpenStream", &CArc::OpenStream, LUA_ARGS(const COpenOptions & op))
            .addFunction("ReOpen", &CArc::ReOpen, LUA_ARGS(COpenOptions &op))
        .endClass()
        .beginClass<CCodecs>("CCodecs")
            .addConstructor(LUA_SP(std::shared_ptr<CCodecs>), LUA_ARGS(void))
        .endClass()
        .beginClass<CReadArcItem>("CReadArcItem")
            .addConstructor(LUA_SP(std::shared_ptr<CReadArcItem>), LUA_ARGS(void))
            .addVariable("Path", &CReadArcItem::Path)
            .addVariable("MainPath", &CReadArcItem::MainPath)
            .addVariable("IsAltStream", &CReadArcItem::IsAltStream)
            .addVariable("WriteToAltStreamIfColon", &CReadArcItem::WriteToAltStreamIfColon)
            .addVariable("IsDir", &CReadArcItem::IsDir)
            .addVariable("MainIsDir", &CReadArcItem::MainIsDir)
            .addVariable("ParentIndex", &CReadArcItem::ParentIndex)
        .endClass()

        .beginClass<COpenCallbackImp>("COpenCallbackImp")
            .addVariable("FileNames", &COpenCallbackImp::FileNames)
            .addVariable("FileSizes", &COpenCallbackImp::FileSizes)
            .addFunction("SetTotal", &COpenCallbackImp::SetTotal, LUA_ARGS(const UInt64 *files, const UInt64 *bytes))
            .addFunction("SetCompleted", &COpenCallbackImp::SetCompleted, LUA_ARGS(const UInt64 *files, const UInt64 *bytes))
        .endClass()
        .beginClass<IInArchive>("IInArchive")
            .addFunction("Open", &IInArchive::Open, LUA_ARGS(IInStream *stream, const UInt64 *, IArchiveOpenCallback *callback))
            .addFunction("Close", &IInArchive::Close)
            .addFunction("Extract", &IInArchive::Extract, LUA_ARGS(const UInt32 *indices, UInt32 numItems, Int32 testMode, IArchiveExtractCallback *extractCallback))
            .addFunction("GetArchiveProperty", &IInArchive::GetArchiveProperty, LUA_ARGS(PROPID propID, PROPVARIANT *value))
            .addFunction("GetNumberOfItems", &IInArchive::GetNumberOfItems, LUA_ARGS(UInt32 *numItems))
        .endClass()
        .beginClass<CArchiveExtractCallback>("CArchiveExtractCallback")
        
        .endClass()
//        .endModule()
		;
        mod.pushToStack();

        return 1;
    }

}
