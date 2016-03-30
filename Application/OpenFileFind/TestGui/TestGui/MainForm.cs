using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace TestGui
{
    public partial class MainForm : Form
    {
        PythonEngine pythonEngine;

        public MainForm()
        {
            InitializeComponent();
            pythonEngine = new PythonEngine(this);
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

      
    }
}
