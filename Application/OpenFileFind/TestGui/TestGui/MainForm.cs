using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Anti_Ransomware
{
    public partial class MainForm : Form
    {
        GUI pythonEngine;

        public MainForm()
        {
            InitializeComponent();
            pythonEngine = new GUI(this);
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            pythonEngine.Close();
        }
    }
}
