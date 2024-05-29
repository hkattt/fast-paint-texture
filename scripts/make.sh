ANIMATE=""

# Parse command line arguments
while [[ $# -gt 0 ]] 
do
    key="$1"

    case $key in
        -ANIMATE)
            ANIMATE="ON"
            shift
            ;;
        *)
            # Unknown option
            echo "Unknown option: $key"
            exit 1
            ;;
    esac
done

# Rebuild the project
cd build

if [ -n "$ANIMATE" ]; then
    echo "cmake -DANIMATE=$ANIMATE .."
    cmake -DANIMATE="$ANIMATE" ..
else
    echo "cmake .."
    cmake ..
fi

make