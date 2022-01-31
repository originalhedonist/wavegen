if [[ $(git add --all --dry-run) ]]; then
    echo "There are changes to be committed"
else
    echo "There are no changes to be committed"
fi
