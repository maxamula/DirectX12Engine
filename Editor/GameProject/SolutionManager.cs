using Editor.Project;
using EnvDTE;
using EnvDTE80;
using HandyControl.Controls;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Editor.GameProject
{
    public enum GameProjectType
    {
        GameAssembly = 1,
        Launcher = 2,
    }
    public static class SolutionManager
    {
        [DllImport("ole32.dll")]
        private static extern int GetRunningObjectTable(uint reserved, out IRunningObjectTable ppInterface);
        [DllImport("ole32.dll")]
        private static extern int CreateBindCtx(uint reserved, out IBindCtx ppInterface);
        public static void OpenVS(string solutionPath)
        {
            IRunningObjectTable rot = null;
            IEnumMoniker moniker = null;
            IBindCtx bind = null;
            try
            {
                if (_vsInstance == null)
                {
                    // check if vs is already opened
                    Debug.Assert(GetRunningObjectTable(0, out rot) == 0);
                    rot.EnumRunning(out moniker);
                    moniker.Reset();
                    Debug.Assert(CreateBindCtx(0, out bind) == 0);
                    IMoniker[] current = new IMoniker[1];
                    while (moniker.Next(1, current, IntPtr.Zero) == 0)
                    {
                        string name = string.Empty;
                        current[0]?.GetDisplayName(bind, null, out name);
                        if (name.Contains("VisualStudio.DTE"))
                        {
                            Debug.Assert(rot.GetObject(current[0], out object obj) == 0 && obj != null);
                            EnvDTE80.DTE2 dte = obj as EnvDTE80.DTE2;
                            // if right solution is opened in visual studio
                            if (dte.Solution.FullName == solutionPath)
                            {
                                _vsInstance = dte;
                                break;
                            }
                        }
                    }
                    // if no visual studio opened create new instance
                    if (_vsInstance == null)
                    {
                        Type vsType = Type.GetTypeFromProgID("VisualStudio.DTE", true);
                        _vsInstance = Activator.CreateInstance(vsType) as EnvDTE80.DTE2;
                    }
                    _events = _vsInstance.Events;
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
            finally
            {
                // Release COM objects
                if (rot != null) Marshal.ReleaseComObject(rot);
                if (moniker != null) Marshal.ReleaseComObject(moniker);
                if (bind != null) Marshal.ReleaseComObject(bind);
            }
        }

        public static void CloseVS()
        {
            try
            {
                if (_vsInstance?.Solution.IsOpen == true)
                {
                    _vsInstance.ExecuteCommand("File.SaveAll");
                    _vsInstance.Solution.Close(true);
                }
                _vsInstance?.Quit();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }

        }

        public static void AddFiles(string solutionPath, string projectName, string[] files)
        {
            OpenVS(solutionPath);
            try
            {
                if (_vsInstance != null)
                {
                    if (!_vsInstance.Solution.IsOpen)
                        _vsInstance.Solution.Open(solutionPath);
                    else _vsInstance.ExecuteCommand("File.SaveAll");

                    // find gamecode project
                    foreach (EnvDTE.Project proj in _vsInstance.Solution.Projects)
                    {
                        if (proj.UniqueName.Contains(projectName))
                        {
                            foreach (var file in files)
                            {
                                proj.ProjectItems.AddFromFile(file);
                            }
                        }
                    }
                    var cpp = files.FirstOrDefault(x => System.IO.Path.GetExtension(x) == ".cpp");
                    _vsInstance.ItemOperations.OpenFile(cpp, "{7651A703-06E5-11D1-8EBD-00A0C90F26EA}").Visible = true;
                    _vsInstance.MainWindow.Activate();
                    _vsInstance.MainWindow.Visible = true;
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }

        public static bool IsBusy()
        {
            try
            {
                return _vsInstance != null && (_vsInstance.Debugger.CurrentProgram != null || _vsInstance.Debugger.CurrentMode == EnvDTE.dbgDebugMode.dbgRunMode);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                return true;
            }
        }

        public static void BuildProject(Project.Project project, GameProjectType type, string buildConfig, params Action[] callbacks)
        {
            _callbacks = callbacks;
            project.IsBuildAvailable = false;
            OpenVS(project.Path + $"{project.Name}\\" + $"{project.Name}.sln");
            if (IsBusy())
            {
                Growl.WarningGlobal("Visual Studio is busy");
                project.IsBuildAvailable = true;
                return;
            }
            try
            {
                if (!_vsInstance.Solution.IsOpen)
                    _vsInstance.Solution.Open(project.Path + $"{project.Name}\\" + $"{project.Name}.sln");
      
                _buildEvents = _events.BuildEvents;
                _buildEvents.OnBuildProjConfigDone += OnBuildDone;
                
                _ClearPdb(project);

                _vsInstance.Solution.SolutionBuild.SolutionConfigurations.Item(buildConfig).Activate();
                EnvDTE80.SolutionBuild2 solutionBuild = (EnvDTE80.SolutionBuild2)_vsInstance.Solution.SolutionBuild;
                EnvDTE.Project proj = _vsInstance.Solution.Projects.Item((int)type);
                _flag = false;
                solutionBuild.BuildProject(solutionBuild.ActiveConfiguration.Name, proj.UniqueName, false);
            }
            catch (Exception ex)
            {
                Growl.ErrorGlobal(ex.Message);
                project.IsBuildAvailable = true;
            }
        }

        private static void OnBuildDone(string proj, string ProjectConfig, string Platform, string SolutionConfig, bool Success)
        {
            if(!_flag)
            {
                _flag = true;
                Project.Project project = null;
                Application.Current.Dispatcher.Invoke(new Action(() => { project = Project.Project.Current; }));
                project.IsBuildAvailable = true;
                if (Success)
                {
                    foreach (var callback in _callbacks) // invoke all callbacks
                    {
                        if (callback != null)
                            callback.Invoke();
                    }
                }
            }    
        }

        private static void _ClearPdb(Project.Project project)
        {
            try
            {
                foreach (var pdb in Directory.GetFiles($@"{project.Path}{project.Name}\x64\Debug\", "*.pdb"))
                    File.Delete(pdb);
                foreach (var pdb in Directory.GetFiles($@"{project.Path}{project.Name}\x64\Release\", "*.pdb"))
                    File.Delete(pdb);
            } catch { }
        }

        private static EnvDTE80.DTE2 _vsInstance = null; // visual studio instance
        private static Events _events;
        private static BuildEvents _buildEvents;
        private static Action[] _callbacks; // callbacks that will be called after build is done and successfull
        private static bool _flag = false;
    }
}
