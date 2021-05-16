# Little script which will handle test run
if [ ! -d "build" ] ; then
    echo "build directory does not exist, please make it and build the tests inside it"
    exit -1
fi

if [ ! -d "build/bin" ] ; then
    echo "No bin directory found in build. Please compile tests before attempting to run them"
    exit -1;
fi

pushd build/bin > /dev/null
echo "############################ Running all tests ... ####################################"
directories=$(find -type d)
directories=("${directories:1}") #removed the 1st element
echo "directories = : $directories"

extension=""
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    extension=".exe"
fi

failed_once=0
failed_tests=""
for dir in $directories
do
    echo -e "\nentering $(basename ${dir}) directory"
    pushd $dir > /dev/null
    fileList=$(find -maxdepth 1 -type f -name *${extension})
    if [ -z $fileList ] ; then
        echo -e "No files in directory"
    else
        for f in $fileList
        do
            echo -e "\n###########################################################\nRunning \033[1;36m$(basename $f) \033[0;0m ... "
            ./$f

            # Push failed tests in list
            if [ $? -ne 0 ] ; then
                failed_once=1
                failed_tests="$failed_tests $(basename $f)"
            fi

        done
    fi
    popd > /dev/null
done

#Prints out failed tests, if any
if [ 0 != $failed_once ] ; then
    echo -e "\n\033[1;31m##########################################################################"
    echo -e "\n\033[1;31mFound failed tests."
    echo -e "\n\033[1;31mList of failed tests :"
    for test in $failed_tests ; do
        echo -e " * $test\n"
    done
    echo -e "\033[0;0m"
fi
