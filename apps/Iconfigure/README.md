# Iconfigure

A command-line utility tool written for setting directories icons visible in Windows Explorer. Tool iterates over subdirectories of a directory specified by the user, looks up the icon and sets it. Currently there are two methods for icon lookup (which can be used simultaneously):

- recursive - each directory is recursively searched for files containing icons. Files are selected by name, though it doesn't have to be a perfect match, e.g. **Gimp** matches **gimp-2.10.exe**.
- icon library - tool looks for icons in directory specified by user. Files are selected by name, requiring perfect match (case insensitive)
