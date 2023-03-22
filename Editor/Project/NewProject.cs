using Editor.GameProject;
using Editor.Utils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace Editor.Project
{
    [DataContract]
    public class ProjectTemplate
    {
        [DataMember]
        public string ProjectType { get; set; }
        [DataMember]
        public string ProjectFile { get; set; }
        [DataMember]
        public List<string> Folders { get; set; }
        public byte[] Icon { get; set; }
        public byte[] Screenshot { get; set; }
        public string ProjectFilePath { get; set; }
        public string IconFilePath { get; set; }
        public string ScreenshotFilePath { get; set; }
        public string TemplatePath { get; set; }
    }
    class NewProject : VMBase
    {
        // Constructor
        public NewProject()
        {
            ProjectTemplates = new ReadOnlyObservableCollection<ProjectTemplate>(_projectTemplates);
            try
            {
                var templateFiles = Directory.GetFiles(_templatePath, "template.xml", SearchOption.AllDirectories);
                Debug.Assert(templateFiles.Any());
                foreach (var file in templateFiles)
                {
                    var template = Utils.Serializer.Deserialize<ProjectTemplate>(file);
                    template.IconFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), "Icon.png"));
                    template.Icon = File.ReadAllBytes(template.IconFilePath);
                    template.ScreenshotFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), "Screenshot.png"));
                    template.Screenshot = File.ReadAllBytes(template.ScreenshotFilePath);
                    template.ProjectFilePath = Path.GetFullPath(Path.Combine(Path.GetDirectoryName(file), "project.ahh"));
                    template.TemplatePath = Path.GetDirectoryName(file);
                    _projectTemplates.Add(template);
                    ValidatePath(); // Validate default path
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }
        // Public
        // Methods
        public string CreateProject(ProjectTemplate template)
        // Returns path to a newly created project file
        {
            if (!ValidatePath())
                return string.Empty;
            if (template == null)
                return string.Empty;
            if (!ProjectPath.EndsWith(Path.DirectorySeparatorChar.ToString()))
                ProjectPath += Path.DirectorySeparatorChar;
            var path = $@"{ProjectPath}{ProjectName}\";
            try
            {
                if (!Directory.Exists(path))
                    Directory.CreateDirectory(path);
                // Create folders for game content etc
                foreach (var folder in template.Folders)
                {
                    Directory.CreateDirectory(Path.GetFullPath(Path.Combine(Path.GetDirectoryName(path), folder)));
                }
                var dirInfo = new DirectoryInfo(path + @".engine\");
                dirInfo.Attributes |= FileAttributes.Hidden;
                // Copy template files to a project path
                File.Copy(template.IconFilePath, Path.GetFullPath(Path.Combine(dirInfo.FullName, "Icon.png")));
                File.Copy(template.ScreenshotFilePath, Path.GetFullPath(Path.Combine(dirInfo.FullName, "Screenshot.png")));

                var projectXml = File.ReadAllText(template.ProjectFilePath);
                projectXml = string.Format(projectXml, ProjectName, ProjectPath);
                var projectPath = Path.GetFullPath(Path.Combine(path, $"{ProjectName}.ahh"));
                File.WriteAllText(projectPath, projectXml);

                // TODO
                CreateMSVCSolution(template, path);

                return path;
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                return string.Empty;
            }

        }

        private void CreateMSVCSolution(ProjectTemplate template, string path)
        {
            Debug.Assert(File.Exists(Path.Combine(template.TemplatePath, "MSVCSolution")));
            Debug.Assert(File.Exists(Path.Combine(template.TemplatePath, "MSVCProject")));

            var solution = File.ReadAllText(Path.Combine(template.TemplatePath, "MSVCSolution"));
              solution = string.Format(solution, "{" + Guid.NewGuid().ToString().ToUpper() + "}", ProjectName, "{" + Guid.NewGuid().ToString().ToUpper() + "}", "{" + Guid.NewGuid().ToString().ToUpper() + "}");
            File.WriteAllText(Path.GetFullPath(Path.Combine(path, $"{ProjectName}.sln")), solution);

            Directory.CreateDirectory(Path.Combine(path, $"GameCode\\GameAssembly"));
            Directory.CreateDirectory(Path.Combine(path, $"GameCode\\Launcher"));
            // GameAssembly
            var projectGA = File.ReadAllText(Path.Combine(template.TemplatePath, "MSVCProject"));
            projectGA = string.Format(projectGA, "GameAssembly", "{" + Guid.NewGuid().ToString().ToUpper() + "}");
            File.WriteAllText(Path.GetFullPath(Path.Combine(path, $"GameCode\\GameAssembly\\GameAssembly.vcxproj")), projectGA);

            // Launcher
            var projectLN = File.ReadAllText(Path.Combine(template.TemplatePath, "MSVCProject"));
            projectLN = string.Format(projectLN, ProjectName, "{" + Guid.NewGuid().ToString().ToUpper() + "}");
            File.WriteAllText(Path.GetFullPath(Path.Combine(path, $"GameCode\\Launcher\\{ProjectName}.vcxproj")), projectLN);
            // Create folders for engine binaries/includes
            Directory.CreateDirectory(Path.Combine(path, $"GameCode\\Engine"));

            //Utils.ResourceWriter.WriteResource("GameProject.Engine.dll", Path.Combine(path, $"GameCode\\Engine\\Engine.dll"));
            //Utils.ResourceWriter.WriteResource("GameProject.Engine.lib", Path.Combine(path, $"GameCode\\Engine\\Engine.lib"));
            var cppmain = Path.GetFullPath(Path.Combine(path, $"GameCode\\Launcher\\main.cpp"));
            GameProject.LauncherMainTemplate launcherTemplate = new GameProject.LauncherMainTemplate();
            var _collection = new ObservableCollection<EngineWindow>(){ new EngineWindow() { Name = "g_mainWindow", Title = "My Window" } };
            launcherTemplate.Session = new Dictionary<string, object>()
            {
                {"windows", new ReadOnlyObservableCollection<EngineWindow>(_collection) }
            };
            launcherTemplate.Initialize();
            using (var sw = File.CreateText(cppmain))
                sw.Write(launcherTemplate.TransformText()); 
            GameProject.SolutionManager.AddFiles(Path.GetFullPath(Path.Combine(path, $"{ProjectName}.sln")), ProjectName, new string[] { cppmain });
            //Utils.ResourceWriter.WriteResource("GameProject.Engine.h", Path.Combine(path, $"GameCode\\Engine\\API\\Engine.h"));
            //Utils.ResourceWriter.WriteResource("Editor.Resources.Engine.Common.h", Path.Combine(path, $"GameCode\\Engine\\Common\\Common.h"));
        }

        // Properties
        public bool IsValid
        {
            get => _isValid;
            set
            {
                if (_isValid != value)
                {
                    _isValid = value;
                    OnPropertyChanged(nameof(IsValid));
                }
            }
        }
        public string ErrorMsg
        {
            get => _errorMsg;
            set
            {
                if (_errorMsg != value)
                {
                    _errorMsg = value;
                    OnPropertyChanged(nameof(ErrorMsg));
                }
            }
        }
        public ReadOnlyObservableCollection<ProjectTemplate> ProjectTemplates { get; }
        public string ProjectName
        {
            get => _projectName;
            set
            {
                if (_projectName != value)
                {
                    _projectName = value;
                    ValidatePath();
                    OnPropertyChanged(nameof(ProjectName));
                }
            }
        }
        public string ProjectPath
        {
            get => _projectPath;
            set
            {
                if (_projectPath != value)
                {
                    _projectPath = value;
                    ValidatePath();
                    OnPropertyChanged(nameof(ProjectPath));
                }
            }
        }
        // Private
        // Methods
        private bool ValidatePath()
        {
            var path = ProjectPath;
            if (!path.EndsWith(Path.DirectorySeparatorChar.ToString()))
                path += Path.DirectorySeparatorChar;
            path += $@"{ProjectName}\";
            IsValid = false;
            if (string.IsNullOrEmpty(ProjectName.Trim()))
            {
                ErrorMsg = "Invalid project name.";
            }
            else if (ProjectName.IndexOfAny(Path.GetInvalidFileNameChars()) != -1)
            {
                ErrorMsg = "Invalid chars are used in project name.";
            }
            else if (string.IsNullOrEmpty(ProjectPath.Trim()))
            {
                ErrorMsg = "Invalid project path.";
            }
            else if (ProjectPath.IndexOfAny(Path.GetInvalidPathChars()) != -1)
            {
                ErrorMsg = "Invalid chars are used in project name.";
            }
            else if (Directory.Exists(path) && Directory.EnumerateFileSystemEntries(path).Any())
            {
                ErrorMsg = "Selected folder is not empty.";
            }
            else
            {
                ErrorMsg = string.Empty;
                IsValid = true;
            }
            return IsValid;
        }
        // Fields
        // Backing fields
        private bool _isValid = true;
        private string _errorMsg = string.Empty;
        private string _projectName = "New_Project";                                                                                                  // Project name
        private string _projectPath = $@"{Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)}\Engine\";                                 // Project path
        private string _templatePath = $@"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}\Engine\";
        private ObservableCollection<ProjectTemplate> _projectTemplates = new ObservableCollection<ProjectTemplate>();                              // Bindable templates collection
    }
}
