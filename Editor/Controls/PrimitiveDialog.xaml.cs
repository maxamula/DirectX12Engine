using System;
using System.Collections.Generic;
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

namespace Editor.Controls
{
    /// <summary>
    /// Логика взаимодействия для PrimitiveDialog.xaml
    /// </summary>
    public partial class PrimitiveDialog : Window
    {
        public PrimitiveDialog()
        {
            InitializeComponent();
        }

        private void OnTypeChanged(object sender, SelectionChangedEventArgs e)
        {

        }
    }
}
