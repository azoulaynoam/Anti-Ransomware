using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;

namespace Anti_Ransomware
{
    public partial class MainForm : Form
    {
        private NotifyIcon  trayIcon;
        private ContextMenu trayMenu;

        //[StructLayout(LayoutKind.Sequential, Pack = 1)]
        //struct variables_t
        //{
        //    string filename;
        //    string action;
        //};

        [DllImport(@"DLL_Files_Change.dll")]
        static extern void Files_Change(string dir_path);//, ref  variables_t variables);



        public MainForm()
        {
            InitializeComponent();
            SysTray();
        }
 
        public void  SysTray()
        {
            // Create a simple tray menu with only one item.
            trayMenu = new ContextMenu();
            trayMenu.MenuItems.Add("Options", OnOpen);
 
            // Create a tray icon. In this example we use a
            // standard system icon for simplicity, but you
            // can of course use your own custom icon too.
            trayIcon      = new NotifyIcon();
            trayIcon.Text = "Anti-Ransomware";
            trayIcon.Icon = new Icon(SystemIcons.Application, 40, 40);
 
            // Add menu to tray icon and show it.
            trayIcon.ContextMenu = trayMenu;
            trayIcon.Visible     = true;
        }
 
        protected override void OnLoad(EventArgs e)
        {
            Visible       = false; // Hide form window.
            ShowInTaskbar = false; // Remove from taskbar.
            base.OnLoad(e);
        }

        private void OnOpen(object sender, EventArgs e)
        {
            Visible = true; // Hide form window.
            ShowInTaskbar = true; // Remove from taskbar.
        }

        public void Run()
        {
            Files_Change("C:\\A");
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            Thread th = new Thread(new ThreadStart(Run));
            th.IsBackground = true;
            th.Start();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                richTextBox1.Text = File.ReadAllText("new.txt");
            }
            catch
            {
                return;
            }
        }
    }
}