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
    /// Логика взаимодействия для SceneView.xaml
    /// </summary>
    public partial class SceneView : UserControl
    {
        public SceneView()
        {
            InitializeComponent();
        }

        private void OnObjectSelectionChanged(object sender, EventArgs e)
        {
            var obj = (sender as TreeView).SelectedItem;
            if(obj is GameObject)
                ComponentsView.Instance.DataContext = obj;
        }

        private void OnAddChildClick(object sender, RoutedEventArgs e)
        {
            Project.SceneGraphBase item = (Project.SceneGraphBase)tree.SelectedItem;
            GameObject obj = item.CreateObject("New Object");
        }

        private void OnRemoveChildClick(object sender, RoutedEventArgs e)
        {
            Project.SceneGraphBase item = (Project.SceneGraphBase)tree.SelectedItem;
            if (item != null && item is GameObject)
            {
                item.Destroy();
            }
        }
    }
}
