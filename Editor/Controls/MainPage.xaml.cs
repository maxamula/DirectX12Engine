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
    /// Логика взаимодействия для MainPage.xaml
    /// </summary>

    public class EditorPage : Page 
    {
        virtual public void Destroy()
        {
            
        }
    }

    public partial class MainPage : EditorPage
    {
        public MainPage()
        {
            InitializeComponent();
            Instance = this;
        }
        public static MainPage Instance = null;
        public override void Destroy()
        {
            viewport.Destroy();
        }

        public HandyControl.Controls.Badge CompileBadge
        {
            get { return compileBadge; }
        }

        private void OnCompileClick(object sender, EventArgs e)
        {
            var project = this.DataContext as Project.Project;
            project.CompileGameAssembly();
            compileBadge.ShowBadge = false;
        }
        private void OnRunClick(object sender, EventArgs e)
        {
            var project = this.DataContext as Project.Project;
            project.RunGameProject();
        }
    }
}
