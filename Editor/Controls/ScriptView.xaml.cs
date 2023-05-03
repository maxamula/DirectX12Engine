using Editor.Components;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Editor.Controls
{
    /// <summary>
    /// Логика взаимодействия для ScriptView.xaml
    /// </summary>
    public partial class ScriptView : UserControl
    {
        public ScriptView()
        {
            InitializeComponent();
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            (this.DataContext as Script).ID = ((sender as ComboBox).SelectedItem as Engine.ScriptInfo).Id;
        }
    }
}
