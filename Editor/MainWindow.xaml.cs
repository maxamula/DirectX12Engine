using Editor.Project;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
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
using System.Diagnostics;
using System.CodeDom.Compiler;
using Editor.GameProject;
using EnvDTE;
using Editor.Utils;
using System.Runtime.InteropServices;
using Application = System.Windows.Application;
using HandyControl.Controls;
using Editor.Controls;
using Editor.Content;

namespace Editor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 
    public enum PageType
    {
        MainPage,
        WindowManager,
    }

    public partial class MainWindow : HandyControl.Controls.Window
    {
        static private EditorPage[] s_pages = null;
        public MainWindow()
        {
            InitializeComponent();
            ShowProjectDialog();
            //load pages
            s_pages = new EditorPage[] 
            { 
                new Controls.MainPage() {DataContext = this.DataContext}, 
                new Controls.WindowManagerPage() {DataContext = this.DataContext}
            };
            frame.Navigate(s_pages[0]);

            // =================== TEST ===================
            //Project.Project project = new Project.Project("NAME HERE");
            //project.AddScene("Default");
            //Utils.Serializer.Serialize(project, "updated.project");
            // =================== TEST ===================
        }

        async private void ShowProjectDialog()
        {
            var projectDialog = new ProjectDialog();    // create project dialog instance
            if (projectDialog.ShowDialog() == false || projectDialog.DataContext == null)
                Application.Current.Shutdown();         // Shutdown application if user clicked exit btn or error 
            else
            {
                Project.Project.Current?.Unload();          // if loading other project - unload old one
                Project.Project project = (Project.Project)projectDialog.DataContext;
                DataContext = project;    // Pass project as context to main window
                project.UpdateGPFiles();
                AssetRegistry.Reset(project.ContentPath);
                await Task.Run(() => project.CompileGameAssembly());
            }
        }

        async private void Window_PreviewKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            var project = this.DataContext as Project.Project;
            if (e.Key == Key.F1)
            {
                //Project.ProjectCoreDialog dialog = new Project.ProjectCoreDialog();
                /*WindowDialog dialog = new WindowDialog();
                dialog.DataContext = this.DataContext;
                dialog.ShowDialog();*/
                drawer.IsOpen = !drawer.IsOpen;
                UpdateLayout();
            }
            else if (e.Key == Key.F2)
            {
                string userInput = Microsoft.VisualBasic.Interaction.InputBox("Enter window procedure name:", "New string", "DefaultWndProc");
                if (string.IsNullOrEmpty(userInput))
                    return;
                project.AddScript(userInput);
            }
            if (e.Key == Key.W) Engine.Viewport.Render();
        }
        private void OnAddSceneBtnClick(object sender, EventArgs e)
        {
            var context = this.DataContext as Project.Project;
            context.AddScene("New_Scene_" + context.Scenes.Count);
        }
        private void OnAddprimitiveClick(object sender, EventArgs e)
        {
            PrimitiveDialog dialog = new PrimitiveDialog();
            dialog.ShowDialog();
        }
        private void OnSaveBtnClicked(object sender, EventArgs e)
        {
            Project.Project.Save(this.DataContext as Project.Project);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Project.Project.Current?.Unload();
            foreach(var page in s_pages)
            {
                page.Destroy();
            }
        }

        async private void CreatePrimitiveClick(object sender, RoutedEventArgs e)
        {
            var dlg = new Controls.PrimitiveDialog();
            dlg.ShowDialog();
        }

        private void CreateWndProcClick(object sender, RoutedEventArgs e)
        {
            var project = this.DataContext as Project.Project;
            string userInput = Microsoft.VisualBasic.Interaction.InputBox("Enter window procedure name:", "New string", "DefaultWndProc");
            if (string.IsNullOrEmpty(userInput))
                return;
            project.AddWndProc(userInput);
        }

        private void SideMenu_SelectionChanged(object sender, HandyControl.Data.FunctionEventArgs<object> e)
        {
            var selectedItem = (SideMenuItem)e.Info;
            var type = (PageType)selectedItem.Tag;
            frame.Navigate(s_pages[(int)type]);
        }
    }
}
