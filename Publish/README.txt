=======================================================================
T-PoT - Total Commander file system plug-in for iPod and iPhone devices
=======================================================================

1. Purpose
----------

T-PoT is a plug-in for Total Commander allowing to browse the Apple iPod
Touch and iPhone contents via USB.

It automatically converts PNG and PLIST files when they are copied from
the iPod to the PC.


2. Requirements
---------------

T-PoT runs on Windows only, it has been tested with Total Commander 7.02a
but should be back-compatible with version 6.

The following software must be installed:

* iTunes 7.4 or later (Windows XP SP2 required).


3. Installation
---------------

1) Normally all you have to do is to open the .zip file with Total Commander
   and it will self-install.

* A pop-up will open asking you whether you want to install the plug-in or
  not, answer "Yes".

* If another version was installed before, you have to choose whether you
  want to overwrite the files or to install the plug-in into another
  directory. In the case you install an update or re-install the plug-in,
  simply choose to overwrite.

  Note that you will be able to overwrite the plug-in even if you have
  opened a connection on the iPod. The new version will correctly replace
  the previous one and you should be able to go on browsing on the device.

* Otherwise an installation directory is suggested, you may modify it if
  you want. Click on OK when you are ready to install the plug-in.

2) If for some reason the self-install doesn't trigger, proceed as follows to
   install it manually:

* Copy T-PoT.wfx to the following directory:

    <TotalCommander>\plugins\wfx

  where "<TotalCommander>" is the Total Commander installation directory,
  e.g. c:\Program Files\totalcmd. You may have to create the wfx
  subdirectory if it doesn't exist yet.

* In Total Commander, choose "Configurations" / "Options..." from the
  menu.

* In the "Configuration" window, select "Plugins", under the "Operation"
  category on the left panel.

* Click on the "Configure" button of the "File system plugins (.WFX)"
  plug-in category, on the right.

* Click the "Add" button and browse to the directory where you have
  copied the T-PoT.wfx file earlier. Select this file and click
  on the "Open" button.

3) In both methods a new plug-in will be shown in the "File system plugins"
   window.

* The default name that will show in the "Network Neighborhood" system
  folder of Total Commander is "T-PoT", but you can rename it by clicking on
  "Rename" if you wish. You can also do that later from the configuration
  menu.

* Click on "OK" when you are done, to close the "File
  system plugins" window, and OK again to close the "Configuration" window.


4. Uninstalling T-PoT
---------------------

To remove the plug-in,

* In Total Commander, choose "Configurations" / "Options..." from the
  menu.

* In the "Configuration" window, select "Plugins", under the "Operation"
  category on the left panel.

* Click on the "Configure" button of the "File system plugins (.WFX)"
  plug-in category, on the right.

* Select the plug-in, and click on the "Remove" button.

This will not remove the files in the Total COmmander installation
directory, however. You can re-install the plug-in from those files
or from the original .zip.


5. User Guide
-------------

To connect to the iPod Touch or iPhone, proceed as follows:

* Connect your iPod Touch or iPhone in an USB port and make sure it is
  switched on and ready. The connection can be established whether
  the device is locked or not.

* Select the "Network Neighborhood" system folder with Total Commander,
  for example by changing the left or right drive and choosing the
  last entry of the list, "\ - Network Neighborhood".

  You should see a "[t-pot]" directory (or another name if you renamed
  the plug-in earlier). Enter that directory and you will be in the
  uppermost directory accessible from the USB port.

Note that if your device has been jailbroken, the top directory will
be the root directory of the iPod/iPhone file system, in other words the
directory / of MacOS X. If the device has not been jailbroken and is in
its original configuration, the top directory will be the Media directory
used by iTunes to store the music, photos and so on, corresponding to the
following directory:

    /private/var/root/Media

To transfer files from and to your iPod, use the normal commands of
Total Commander.

##########################################################################
                                 CAUTION
##########################################################################

A) Be careful about which files you write to the device and where.

Overwriting system files may render your device non operable and force
you to restore it. Doing so will make you lose all your applications
and data.

B) The USB API provided by iTunes does not allow a full visibility on the
file system of the device. Among others, the following attributes are
not accessible:

* read/write/execute permissions
* date of creation/modification/access/...

Which means that if you copy from the iPod to the PC, you will lose some
part of the information. While this is not important to copy most of the
data (music, pdf files, plist files, png files, ...), this can have a
consequence if you use this plug-in for to make a backup of an entire
directory structure that includes soft links and executable files:
on transferring the structure back to the iPod, soft links will be copied
as regular files/directories, and execute permissions will be reset.

The default permissions set on files copied to the iPod/iPhone are:
* read/write for owner
* read for group and everyone

The date is set to the instant of the transfer, and files are created on
the device as regular files or directories.

A way to ensure that the file attributes are preserved is to log on the
device with an SSH session, and to make a tarball archive which can then
be transferred by USB (which is faster). For that, your device needs
to be jailbroken.

For example, to save the contents of /var/root/Library and
/System/Library:

* Install the BSD Subsystem on your iPod/iPhone, this application by
  Nullriver provides useful Mac OS X commands like tar and gzip.
* Log in as root using an SSH session, using WinSCP or PuTTY for instance.
* Type in the following commands:

    cd /tmp
    tar czf backup.tgz /var/root/Library /System/Library

* Log off when the process has finished creating the archive.
* Copy the /tmp/backup.tgz file to your PC with Total Commander.
* Delete the /tmp/backup.tgz on your iPod or iPhone.

Restoring the whole archive or part of it can be done by using a similar
procedure: copying back the tarball to /etc and using tar to extract
the desired contents.

THIS IS AN EXAMPLE ONLY to illustrate the procedure, once again make sure
you know what you are doing and choose the directories you save/restore
carefully. Overwriting system files might force you to restore your
device thus losing all its applications and data!

##########################################################################


6. Known Issues
---------------

* "Create new text file and load into editor" (Shift-F4) doesn't work as
  expected, it will create a file on the other panel (left vs right) if
  it points to the PC file system. This is an issue on the side of
  Total Commander and its author is working on the problem.

* "Synchronize Dirs" shows files as unknown (?) in a comparison even if
  the date is ignored and the comparison is done by contents.

* Permissions and date are lost during the transfer. See the CAUTION
  note above. This is a limitation of the API provided by iTunes and
  there is no known work-around. If you happen to know one, please let
  me know.

A public repository is set up on Google Code:
http://code.google.com/p/t-pot/

Also, if you have remarks or problems using the plugin, try sending a PM
to Scythal on the following site:
http://www.ipodtouchfans.com/forums/


7. History
----------

* Version 1.3 (28-Apr-2010)

    Added translation of UUIDs in the Applications folder to friendly app names.

* Version 1.2 (21-Feb-2010)

    Added codepage conversion for filenames (UTF-8-MAC <-> ANSI).
    Added configuration dialog, file properties dialog. Symlinks are handled
    more or less correctly (symlinks to dirs are reported as ordinary dirs
    to make things easier). Also, tested with OS firmware 3.0.

* Version 1.1 (23-Jan-2007)

    Local file-to-file copy on the iPod. This is only for the sake of
    completeness, it is recommended to use an iPod native application to
    copy files locally to preserve the permission flags. Doing so with
    Total Commander will result in resetting those permission flags,
    because of the limitations explained above.

    Ability to copy files bigger than 4GB (not fully tested).

* Version 1.0 (30-Dec-2007)

    Plist translation code has been redone from scratch, based on the Apple
    CoreFoundation references. It's now simpler - at least as much as the
    Apple way would allow... - and doesn't leak memory like the previous
    one which was based on an existing hack.

* Version 0.4 (23-Dec-2007)

    Self-install version.

* Version 0.3 (20-Dec-2007)

    Added conversion of property list binary files to text files.

    Most of the plist files originally on the iPod - used or generated
    by the original applications - are stored in binary format to save
    space. The plug-in now automatically converts them to text (XML)
    when they are transferred from the iPod to the PC.

    This is also done when they are compared, viewed or edited, since this
    implies a copy to a temporary file on the PC.

* Version 0.2 (11-Dec-2007)

    Added conversion of PNG image files.

    Apple uses a non-compliant PNG format to store the images on the iPod.
    The plug-in automatically generates proper PNG files when they are
    copied from the iPod to the PC.

    For some reason, the PNG images found in the original applications have
    a new chunk type "CgBl" which signals different modifications in the
    IDAT chunks:

        * No header/footer in the zlib-deflated data,
        * Inversion of the red and blue channels.

    Besides, this CgBl chunk is found before the IHDR chunk, which should
    normally be in the first position.

* Version 0.1 (10-Dec-2007)

    First public test version.

8. Licence
----------

Copyright (c) 2007-2009, Scythal
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the software nor the names of its contributors may be
      used to endorse or promote products derived from this software without
      specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ITS AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.


9. Trademark and Copyright Statements
-------------------------------------

* Apple, iPod, iPhone, iTunes and Mac OS are trademarks of Apple Computer, Inc.,
  registered in the U.S. and other countries. All rights reserved.
* Total Commander is Copyright (C) 1993-2007 by Christian Ghisler, C. Ghisler & Co.
* WinSCP is Copyright (C) 2000-2007 by Martin Prikryl.
* PuTTY is Copyright (C) 1997-2007 by Simon Tatham.

All mentioned Trademarks and Copyrights belong to their respective owners.


10. Links
---------

* T-PoT public repository

    http://code.google.com/p/t-pot/

* Total Commander

    http://www.ghisler.com/

* Apple

    http://www.apple.com/

* WinSCP

    http://winscp.net/

* PuTTY

    http://www.chiark.greenend.org.uk/~sgtatham/putty/
