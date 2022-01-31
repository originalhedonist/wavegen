# check for outstanding commits instead of just committing them?
# if [[ $(git add --all --dry-run) ]]; then
#    echo "There are changes to be committed - please commit them first"
# else    
# ...
# fi

json=$1.json
if [ ! -f $json ]; then
    echo $json" does not exist."
    exit 1
fi

commitmessage="Committing all files prior to making "$1
echo $commitmessage
git add --all && git commit -m "$commitmessage"
commit=`git rev-parse --short HEAD`
wavname=$1_$commit.wav
x64/Release/wavegen.exe $json $wavname
lame $wavname