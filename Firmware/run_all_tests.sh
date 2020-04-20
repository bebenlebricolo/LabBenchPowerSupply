# Little script which will handle test run
if [ ! -d "Tests/build" ] ; then
    echo "build directory does not exist, please make it and build the tests inside it"
    exit -1
fi

if [ ! -d "Tests/build/bin" ] ; then
    echo "No bin directory found in build. Please compile tests before attempting to run them"
    exit -1;
fi

pushd Tests/build/bin > /dev/null
echo "############################ Running all tests ... ####################################"
directories=$(find -type d)
directories=("${directories:1}") #removed the 1st element
echo "directories = : $directories"

for dir in $directories  
do 
    echo -e "\nentering ${dir} directory"
    pushd $dir > /dev/null
    for f in $(find -type f) 
    do
        echo -e "\n###########################################################\nRunning $f ... "
        ./$f
    done
    popd > /dev/null
done
