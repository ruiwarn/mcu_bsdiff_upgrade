from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class helloRecipe(ConanFile):
    name = "bsdiff_upgrade"
    version = "1.0.3"
    user = "xian"
    channel = "stable"  #"stable" or "testing"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "readme.md", "bsdiff/*", "lib/*", "lzma/*", "user/*"

    #依赖包
    #requires = "cmsis-dsp/5.8.0@xian/stable"

    # Optional metadata
    license = "zc"
    author = "wangrui"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "static libraries"

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": False}

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")
		#del self.settings.compiler.cppstd
		#del self.settings.compiler.libcxx

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        if self.settings.compiler == "keil":
            if self.settings.arch == "Cortex-M4F":
                tc.variables["C_FLAGS"] = "--cpu Cortex-M4.fp.sp -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["CXX_FLAGS"] = "--cpu Cortex-M4.fp.sp -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["ASM_FLAGS"] = "--cpu Cortex-M4.fp.sp -g --apcs=interwork --pd '__MICROLIB SETA 1'"
            elif self.settings.arch == "Cortex-M4":
                tc.variables["C_FLAGS"] = "--cpu Cortex-M4 -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["CXX_FLAGS"] = "--cpu Cortex-M4 -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["ASM_FLAGS"] = "--cpu Cortex-M4 -g --apcs=interwork --pd '__MICROLIB SETA 1'"
            elif self.settings.arch == "Cortex-M3":
                tc.variables["C_FLAGS"] = "--cpu Cortex-M3 -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["CXX_FLAGS"] = "--cpu Cortex-M3 -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["ASM_FLAGS"] = "--cpu Cortex-M3 -g --apcs=interwork --pd '__MICROLIB SETA 1'"
            elif self.settings.arch == "Cortex-M0":
                tc.variables["C_FLAGS"] = "--cpu Cortex-M0 -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["CXX_FLAGS"] = "--cpu Cortex-M0 -O3 --c99 -c -D__MICROLIB --apcs=interwork --split_sections --diag_suppress=186,66"
                tc.variables["ASM_FLAGS"] = "--cpu Cortex-M0 -g --apcs=interwork --pd '__MICROLIB SETA 1'"
        elif self.settings.compiler == "iar":
            if self.settings.arch == "Cortex-M4F":
                tc.variables["C_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M4 --fpu=VFPv4_sp"
                tc.variables["CXX_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M4 --fpu=VFPv4_sp"
                tc.variables["ASM_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M4 --fpu=VFPv4_sp"
            elif self.settings.arch == "Cortex-M4":
                tc.variables["C_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M4"
                tc.variables["CXX_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M4"
                tc.variables["ASM_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M4"
            elif self.settings.arch == "Cortex-M0":
                tc.variables["C_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M0"
                tc.variables["CXX_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M0"
                tc.variables["ASM_FLAGS"] = "--silent --no_code_motion --endian=little --cpu=Cortex-M0"
        elif self.settings.compiler == "arm-gcc":
            if self.settings.arch == "Cortex-M4F": #带fpu
                tc.variables["C_FLAGS"] = "-x c -mcpu=cortex-m4 -mthumb -Wstack-protector -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu99 -fdata-sections -ffunction-sections --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code -Werror"
                tc.variables["CXX_FLAGS"] = "-x c -mcpu=cortex-m4 -mthumb -Wstack-protector -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu99 -fdata-sections -ffunction-sections --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code -Werror"
                tc.variables["ASM_FLAGS"] = "-x assembler -c -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=gnu99 --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code"
            elif self.settings.arch == "Cortex-M4":  #不带fpu
                tc.variables["C_FLAGS"] = "-x c -mcpu=cortex-m4 -mthumb -Wstack-protector -std=gnu99 -fdata-sections -ffunction-sections --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code -Werror"
                tc.variables["CXX_FLAGS"] = "-x c -mcpu=cortex-m4 -mthumb -Wstack-protector -std=gnu99 -fdata-sections -ffunction-sections --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code -Werror"
                tc.variables["ASM_FLAGS"] = "-x assembler -c -mcpu=cortex-m4 -mthumb -std=gnu99 --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code"
            elif self.settings.arch == "Cortex-M0":  #不带fpu
                tc.variables["C_FLAGS"] = "-x c -mcpu=cortex-m0 -mthumb -Wstack-protector -std=gnu99 -fdata-sections -ffunction-sections --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code -Werror"
                tc.variables["CXX_FLAGS"] = "-x c -mcpu=cortex-m0 -mthumb -Wstack-protector -std=gnu99 -fdata-sections -ffunction-sections --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code -Werror"
                tc.variables["ASM_FLAGS"] = "-x assembler -c -mcpu=cortex-m0 -mthumb -std=gnu99 --param max-inline-insns-single=500 -Wall -Wunused -Wuninitialized -Wformat -Wformat-security -Wunknown-pragmas -Wundef -Wbad-function-cast -Wwrite-strings -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wunreachable-code"
            else:
                print(f"arch发生错误: no arch.")
                sys.exit(1)  # 退出并返回一个非零状态码
        elif self.settings.compiler == "gcc":
                tc.variables["C_FLAGS"] = "-x c -fdata-sections -ffunction-sections -Wall -std=gnu99"
                tc.variables["CXX_FLAGS"] = "-x c -fdata-sections -ffunction-sections -Wall -std=gnu99"
                tc.variables["ASM_FLAGS"] = ""   
                tc.variables["CMAKE_C_COMPILER_WORKS"] = 1
                tc.variables["CMAKE_CXX_COMPILER_WORKS"] = 1
                tc.variables["WITH_WINDOWS_GCC"] = True
        else:
            print(f"compiler发生错误: no compiler.")
            sys.exit(1)  # 退出并返回一个非零状态码
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        
    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = [self.name]













    
