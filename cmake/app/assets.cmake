# battery embed
#CPMAddPackage("gh:batterycenter/embed@1.2.19")
CPMAddPackage(
        NAME b_embed
        GITHUB_REPOSITORY batterycenter/embed
        GIT_TAG v1.2.19
        GIT_SHALLOW
        OPTIONS
        "B_PRODUCTION_MODE ON"
)

