mkdir -p build
cd build
cmake .. -A x64

if [ $? != 0 ]; then
    echo
    echo -n "Press enter to continue..."
    read _
fi
