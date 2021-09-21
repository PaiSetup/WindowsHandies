$version = Read-Host -Prompt "Input release version: "
$buildDir = "build"
$binaryDir = "$buildDir\bin\Release"

# Run CMake and compile
mkdir $buildDir -Force | out-null
cmake -S . -B $buildDir
cmake --build $buildDir --config Release

# Create additional files
cp WindowsHandies.ico $binaryDir
echo $version > $binaryDir\version.txt

# Pack
Compress-Archive -Path $binaryDir\* -DestinationPath WindowsHandies -Update
