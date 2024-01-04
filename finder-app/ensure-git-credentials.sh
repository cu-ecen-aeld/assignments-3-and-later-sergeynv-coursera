ensure-git-credentials() {
    git_name=$(git config --local user.name)
    if [ -z "${git_name}" ]
    then
        echo "Setting git user.name to 'nobody'"
        git config user.name "nobody"
    else
        echo "user.name already set to '${git_name}'"
    fi

    git_email=$(git config --local user.email)
    if [ -z "${git_email}" ]
    then
        echo "Setting git user.email to 'nobody@abc.xyz'"
        git config user.email "nobody@abc.xyz"
    else
        echo "user.email already set to '${git_email}'"
    fi
}
