mkdir build
cd build
cmake ..
make
if [ $? -eq 0 ]; then
    echo "Build successful"
else
    echo "Build failed"
    exit 1
fi
mv inodeyou ../
cd ..
rm -rf build