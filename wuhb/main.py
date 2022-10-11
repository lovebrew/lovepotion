import os
import tkinter

from PIL import Image
from tkinter import filedialog, messagebox, Tk, ttk

# Define valid image file extensions.
file_extensions = [
    ["All Files", ".bmp .jpg .jpeg .png .tga"],
    ["BMP Files", ".bmp"],
    ["JPG Files", ".jpg .jpeg"],
    ["PNG Files", ".png"],
    ["TGA Files", ".tga"],
]

# Check to make sure RPX is provided before building.
def check_package_status():
    if rpx_path.get():
        package_wuhb_button.config(state="normal")
    else:
        package_wuhb_button.config(state="disabled")

# Locate a /content path if necessary.
def select_content_path():
    folder_directory = filedialog.askdirectory()

    if folder_directory:
        content_path.config(state="normal")
        content_path.delete(0, "end")
        content_path.insert(0, folder_directory)
        content_path.config(state="readonly")

# Select the RPX file, the only necessary requirement.
def select_rpx_file():
    file_directory = filedialog.askopenfilename(
        filetypes=[("RPX Files", ".rpx")])

    if file_directory:
        rpx_path.config(state="normal")
        rpx_path.delete(0, "end")
        rpx_path.insert(0, file_directory)
        rpx_path.config(state="readonly")

    check_package_status()

# Select the icon that shows up on the Wii U Menu.
def select_icon_file():
    file_directory = filedialog.askopenfilename(filetypes=file_extensions)

    if file_directory:
        application_image = Image.open(file_directory)
        if application_image.size != (128, 128):
            messagebox.showwarning("WUHB Packager",
                "The picture you selected is not the correct size, so it will "
                + "be resized during packaging to fit correctly.")

        icon_path.config(state="normal")
        icon_path.delete(0, "end")
        icon_path.insert(0, file_directory)
        icon_path.config(state="readonly")

# Select the splash screen image for the television.
def select_tv_splash_file():
    file_directory = filedialog.askopenfilename(filetypes=file_extensions)

    if file_directory:
        application_image = Image.open(file_directory)
        if application_image.size != (1280, 720):
            messagebox.showwarning("WUHB Packager",
                "The picture you selected is not the correct size, so it will "
                + "be resized during packaging to fit correctly.")

        tv_splash_path.config(state="normal")
        tv_splash_path.delete(0, "end")
        tv_splash_path.insert(0, file_directory)
        tv_splash_path.config(state="readonly")

# Select the splash screen image for the Wii U GamePad.
def select_drc_splash_file():
    file_directory = filedialog.askopenfilename(filetypes=file_extensions)

    if file_directory:
        application_image = Image.open(file_directory)
        if application_image.size != (854, 480):
            messagebox.showwarning("WUHB Packager",
                "The picture you selected is not the correct size, so it will "
                + "be resized during packaging to fit correctly.")

        drc_splash_path.config(state="normal")
        drc_splash_path.delete(0, "end")
        drc_splash_path.insert(0, file_directory)
        drc_splash_path.config(state="readonly")

# Where all the core functions happen, packaging to a WUHB.
def package_wuhb_file():
    global wuhbtool_file

    rpx = rpx_path.get()
    content = content_path.get()
    name = long_name_path.get()
    short_name = short_name_path.get()
    author = author_path.get()
    icon = icon_path.get()
    tv_image = tv_splash_path.get()
    drc_image = drc_splash_path.get()

    wuhb = filedialog.asksaveasfilename(
        filetypes=[("WUHB Files", ".wuhb")], defaultextension=".wuhb")

    if wuhb:
        if not name or not short_name or not author:
            answer = messagebox.askyesno(title="WUHB Packager",
                message="One or more fields of metadata are empty. Instead, any missing "
                + "information will be filled with placeholder text. Is this okay?")
            if not answer:
                return

        if not icon or not tv_image or not drc_image:
            answer = messagebox.askyesno(title="WUHB Packager",
                message="One or more image files are missing. Instead, placeholder "
                + "image files will be used during packaging. Is this okay?")
            if not answer:
                return
        
        command = f"{wuhbtool_file} \"{rpx}\" \"{wuhb}\""

        if content:
            # Check for optional /content folder.
            command += f' --content="{content}"'
        if name:
            # Check for optional application name.
            command += f' --name="{name}"'
        if short_name:
            # Check for optional application short name.
            command += f' --short-name="{short_name}"'
        if author:
            # Check for optional application author.
            command += f' --author="{author}"'
        if icon:
            # Check for optional icon image.
            command += f' --icon="{icon}"'
        if tv_image:
            # Check for optional television splash image.
            command += f' --tv-image="{tv_image}"'
        if drc_image:
            # Check for optional Wii U GamePad splash image.
            command += f' --drc-image="{drc_image}"'

        # Disable the package button, run the command, and inform when finished.
        package_wuhb_button.config(text="Processing Package", state="disabled")

        os.system(command)
        messagebox.showinfo("WUHB Packager", "The application was packaged successfully! "
        + "If something seems incorrect, please try again or report the issue to the "
        + "WUHB Packager repository page.")
        package_wuhb_button.config(text="Package WUHB File", state="normal")

# Create the Tkinter window and assets.
main_window = Tk()
main_window.title("WUHB Packager")
main_window.resizable(False, False)
# /content path selection panel.
content_label = tkinter.Label(main_window, text="Path to /content Directory")
content_label.grid(column=0, row=1)
content_path = tkinter.Entry(main_window, state="readonly", width=40, border=5)
content_path.grid(column=0, row=2)
pick_content_path = tkinter.Button(main_window, text="Select Path to /content Directory",
    border=5, command=select_content_path)
pick_content_path.grid(column=0, row=3)
pick_content_path.config(height=1, width=35)
# Program title panel.
title_label = tkinter.Label(main_window, text="WUHB Packager (v1.0.2)", width=40)
title_label.grid(column=1, row=2)
# RPX file selection panel.
rpx_label = tkinter.Label(main_window, text="Path to RPX file")
rpx_label.grid(column=2, row=1)
rpx_path = tkinter.Entry(main_window, state="readonly", width=40, border=5)
rpx_path.grid(column=2, row=2)
pick_rpx_file = tkinter.Button(main_window, text="Select RPX File",
    border=5, command=select_rpx_file)
pick_rpx_file.grid(column=2, row=3)
pick_rpx_file.config(height=1, width=35)
# Application name entry panel.
long_name_label = tkinter.Label(main_window, text="Long Application Name")
long_name_label.grid(column=0, row=5)
long_name_path = tkinter.Entry(main_window, state="normal", width=40, border=5)
long_name_path.grid(column=0, row=6)
# Application short name entry panel.
short_name_label = tkinter.Label(main_window, text="Short Application Name")
short_name_label.grid(column=1, row=5)
short_name_path = tkinter.Entry(main_window, state="normal", width=40, border=5)
short_name_path.grid(column=1, row=6)
# Application author entry panel.
author_label = tkinter.Label(main_window, text="Application Author")
author_label.grid(column=2, row=5)
author_path = tkinter.Entry(main_window, state="normal", width=40, border=5)
author_path.grid(column=2, row=6)
# Application icon selection panel.
icon_label = tkinter.Label(main_window, text="Application Icon (128x128)")
icon_label.grid(column=0, row=8)
icon_path = tkinter.Entry(main_window, state="readonly", width=40, border=5)
icon_path.grid(column=0, row=9)
pick_icon_file = tkinter.Button(main_window, text="Select Application Icon",
    border=5, command=select_icon_file)
pick_icon_file.grid(column=0, row=10)
pick_icon_file.config(height=1, width=35)
# Television splash image selection panel.
tv_splash_label = tkinter.Label(main_window, text="TV Splash Screen Image (1280x720)")
tv_splash_label.grid(column=1, row=8)
tv_splash_path = tkinter.Entry(main_window, state="readonly", width=40, border=5)
tv_splash_path.grid(column=1, row=9)
pick_tv_splash_file = tkinter.Button(main_window, text="Select TV Splash Screen Image",
    border=5, command=select_tv_splash_file)
pick_tv_splash_file.grid(column=1, row=10)
pick_tv_splash_file.config(height=1, width=35)
# Wii U GamePad splash image selection panel.
drc_splash_label = tkinter.Label(main_window, text="DRC Splash Screen Image (854x480)")
drc_splash_label.grid(column=2, row=8)
drc_splash_path = tkinter.Entry(main_window, state="readonly", width=40, border=5)
drc_splash_path.grid(column=2, row=9)
pick_drc_splash_file = tkinter.Button(main_window, text="Select DRC Splash Screen Image",
    border=5, command=select_drc_splash_file)
pick_drc_splash_file.grid(column=2, row=10)
pick_drc_splash_file.config(height=1, width=35)
# Line seperator to put the packaging button away from the rest.
button_separator = ttk.Separator(main_window, orient="horizontal")
button_separator.grid(column=1, row=11, sticky="ew", pady=10)
# Button that will activate the function to package to WUHB.
package_wuhb_button = tkinter.Button(main_window, text="Package WUHB File",
    border=5, command=package_wuhb_file)
package_wuhb_button.grid(column=1, row=12)
package_wuhb_button.config(height=2, width=35)
# Line seperator to put the credits line away from the rest.
button_separator = ttk.Separator(main_window, orient="horizontal")
button_separator.grid(column=1, row=13, sticky="ew", pady=10)
# Credits line to show who made the application.
credit_label = tkinter.Label(main_window, text="Created by NoahAbc12345", width=40)
credit_label.grid(column=1, row=14)

# Make sure packaging button is disabled on start-up.
check_package_status()

if os.name not in ["nt", "posix"]:
    # The user is using an operating system that probably isn't supported.
    messagebox.showerror("WUHB Packager", "Homebrew cannot be packaged on "
        + "this system. Please try again on a Linux or Windows operating system.")
    os._exit(0)

try:
    if os.name == "nt":
        import winreg
        # Confirmed that the user is running Windows.
        registry_key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
            r"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\devkitProUpdater",
            access=winreg.KEY_READ | winreg.KEY_WOW64_32KEY)
        install_location = winreg.QueryValueEx(registry_key, "InstallLocation")
        wuhbtool_file = install_location[0] + r"\tools\bin\wuhbtool.exe"
    else:
        # Confirmed that the user is running Linux.
        wuhbtool_file = os.environ["DEVKITPRO"] + "/tools/bin/wuhbtool"

    if not os.path.exists(wuhbtool_file):
        raise FileNotFoundError()

except (FileNotFoundError, KeyError) as error:
    # wuhbtool might not be installed or configured correctly.
    messagebox.showerror("WUHB Packager", "You must have wuhbtool "
        + "installed properly in order to package an application. Please check "
        + "the WUHB Packager repository page for more information.")
    os._exit(0)

main_window.mainloop()
