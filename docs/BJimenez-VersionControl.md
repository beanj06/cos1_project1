# Instructions

Update this document where indicated [look for the brackets!]. Replace text inside the brackets with your own information. For example: Course Name should be the name of this course, and not the generic words "Course Name".

<br>

## COS119

- ** Benjamin Jimenez **
- ** 10/4/2026 **

This paper addresses some of the topic matter covered in research and activity this week. Be sure to include reference links below to the research and information you used to complete this assignment.

## Topic: Terminal

Professional developers use Terminal daily. It's essential to understand some fundamental commands to use the application.

Update the information below to demonstrate your knowledge on this topic.

**1. Using Terminal, there are essential commands to know.**

List the correct Terminal commands to do the actions listed below. Replace **CMD** with the correct command sequence. You can keep or enhance the brief description.

**The last bullet provides an example**.

- Clear: Clear the Screen
- pwd: Print the "Working Directory"
- ls: List files and folders
- ls -a: List files and folders, including invisible files
- ls -lah: List all files and folders, in human readable form
- cd: Change directory
- cd /: Change directory, go to root directory
- cd ~: Change directory and go to user home directory
- cd ..: Change directory, go up one folder level
- cd ../..: Change directory, go up two folder levels
- cd ~/Desktop: Change directory to my desktop!

**2. Using Terminal...**

**Folder Drop:** Try typing "cd" followed by a space, and then drag a folder into terminal and press return. Test this out and describe your results below.

When I typed cd  and dragged a folder into the Terminal window, Terminal automatically filled in the full path to that folder (for example, cd /Users/yourname/Documents/MyProject). After I pressed return, my working directory changed to that folder, and running pwd confirmed it. This is a handy shortcut because I don't have to type out or remember long folder paths, and it avoids typos in folder names.

## Topic: Version Control & Git

Version control, also known as revision control, records changes to a file or set of files over time so that you can recall specific versions later. In this class, we are learning Git. Update the information below where indicated.

**1. There are three types of version control.**

1. Local version control: Changes are tracked in a simple database on a single computer. This could be as basic as copying files into dated folders, or a tool like RCS that stores patch sets. It is easy to set up, but it offers no collaboration, and if the computer fails, the history is lost.
2. Centralized version control (CVCS): A single central server holds all of the versioned files, and developers check files out from it (examples: Subversion and Perforce). Everyone can see what others are working on, and administration is simple. The downside is that the server is a single point of failure, and if it goes down, nobody can collaborate or save versioned changes.
3. Distributed version control (DVCS): Every developer clones the entire repository, including its full history, to their own machine (examples: Git and Mercurial). Work can be done offline, and every clone acts as a complete backup. This is the model Git uses, and GitHub is a hosting service built on it.

**2. Using Terminal, there are also essential Git commands to know.**

List the correct Git commands to do the actions listed below in Terminal. Replace CMD with the correct command and keep or enhance the brief description.

- git clone <url>: Clone a repository
- git config --global user.name "your user name": Set-up a global user name
- git config --global user.email "you@example.com": Set-up a global email address (to match my GitHub account email)
- git status: Shows the current state of your directory and staging area
- git add <file>: Add modified files to the next commit
- git commit -m "your message": Make a commit with a new message
- git log: Show my commit history
- git --help: Show Git's help screen

**3. Connecting to GitHub using Terminal.**
HTTPS is the the correct way to connect to GitHub in this course. Describe how you connect to GitHub from Terminal using this protocol. What steps do you take?

1. Set my identity once with git config --global user.name and git config --global user.email, using the same email as my GitHub account.
2. On GitHub, open the repository, click the green Code button, select the HTTPS tab, and copy the URL (it starts with https://github.com/).
3. In Terminal, use cd to go to the folder where I want the project to live (for example, cd ~/Desktop).
4. Run git clone <the copied HTTPS URL>. When prompted, I sign in with my GitHub username and a personal access token, since GitHub no longer accepts account passwords for Git operations. macOS can store these credentials in the Keychain so I'm not asked every time.
5. Move into the new folder with cd <repository name>.
6. Do my work, then run git status, git add, and git commit -m "message" to save changes locally.
7. Run git push to send my commits to GitHub, and git pull to download updates from GitHub.

**4. Using .gitignore and Why it's Important**  
Most repositories contain a .gitignore file.

- What is the purpose of this file?
  <br>
  A .gitignore file tells Git which files and folders to leave out of version control. Git won't track, stage, or commit anything that matches the patterns listed in it. This keeps repositories clean, small, and free of files that don't belong in a shared project.

- What is the "**.DS_Store**" file and why would you want to ignore it?
  <br>
  .DS_Store is a hidden file macOS creates in folders to store Finder settings such as icon positions, view options, and window size. It has nothing to do with the project's code, and it differs from computer to computer. Committing it clutters the repository with meaningless changes and can cause needless conflicts between collaborators, so it should be ignored.

- What other file or folder would you want to add to a .gitignore file and why?
  <br>
  I would add node_modules/ (downloaded dependencies that can be reinstalled from package.json), and .env files (which hold secrets such as API keys and passwords that should never be public). I would also add build output folders (like build/ or dist/) and editor or IDE settings folders (like .vscode/), because they are generated automatically or are specific to one person's setup.

<br>

# Reference Links

Replace the example references below with your own links and recommended resources. It is acceptable to provide multiple links for a single topic and to use material provided to you in this class. You are encouraged to link to your own independent research as well.

[ Research Summary: What resource(s) did you find most helpful this past week and why? ]

**Terminal Commands**  
https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/windows-commands

**Three Types of Version Control**  
https://git-scm.com/book/en/v2/Getting-Started-About-Version-Control

**Git Commands**  
https://git-scm.com/docs

**Connecting to GitHub using Terminal**  
https://docs.github.com/en/get-started/git-basics/about-remote-repositories

**Using .gitignore and Why it's Important**  
https://docs.github.com/en/get-started/git-basics/ignoring-files
