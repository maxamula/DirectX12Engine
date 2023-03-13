using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Editor.Project
{
    /// <summary>
    /// Логика взаимодействия для ProjectDialog.xaml
    /// </summary>
    public partial class ProjectDialog : Window
    {
        public ProjectDialog()
        {
            InitializeComponent();
        }
        protected override void OnClosing(CancelEventArgs e)
        {
            e.Cancel = false;
            base.OnClosing(e);
        }

        private void OnToggleBtnClick(object sender, EventArgs e)
        {
            if (sender == openBtn)
            {
                if (createBtn.IsChecked == true)
                {
                    createBtn.IsChecked = false;
                    dialogBody.Margin = new Thickness(0, 0, 0, 0);
                }
                openBtn.IsChecked = true;
            }
            else
            {
                if (openBtn.IsChecked == true)
                {
                    openBtn.IsChecked = false;
                    dialogBody.Margin = new Thickness(-800, 0, 0, 0);
                }
                createBtn.IsChecked = true;
            }
        }
    }
}
