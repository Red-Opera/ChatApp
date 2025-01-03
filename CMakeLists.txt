﻿# CMakeList.txt: CryptologyMessage에 대한 CMake 프로젝트, 여기에 소스를 포함하고
# 프로젝트 특정 논리를 정의합니다.
#
cmake_minimum_required(VERSION 3.12)

# 지원되는 경우 MSVC 컴파일러에 대해 핫 다시 로드 사용하도록 설정합니다.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CryptologyMessage")

# 이 프로젝트의 실행 파일에 소스를 추가합니다.
add_executable (CryptologyMessage 
  "CryptologyMessage.cpp" 
  "CryptologyMessage.h" 
  "Utill/Log.cpp" 
  "Utill/Log.h" 
  "Run.cpp"  
  "Cryptology/StrToImage.h" 
  "Cryptology/StrToImage.cpp" 
  "Utill/TimeUtil.h" 
  "Utill/TimeUtil.cpp" 
  "Utill/TimeScheduler.h" 
  "Utill/TimeScheduler.cpp"
  "Cryptology/AESDefine.h" 
  "Cryptology/AES.h" 
  "Cryptology/AES.cpp"
 "Cryptology/RSA.h" "Cryptology/RSADefine.h" "Cryptology/RSA.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CryptologyMessage PROPERTY CXX_STANDARD 20)
endif()

# Find Threads package
find_package(Threads REQUIRED)

# Link pthread library to the executable
target_link_libraries(CryptologyMessage PRIVATE Threads::Threads)
