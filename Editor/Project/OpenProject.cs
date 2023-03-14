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

namespace Editor.Project
{
    [DataContract]
    class ProjectData
    {
        [DataMember]
        public string ProjectName { get; set; }
        [DataMember]
        public string ProjectPath { get; set; }
        [DataMember]
        public DateTime LastOpened { get; set; }
        public string FullPath { get => $"{ProjectPath}{ProjectName}.ahh"; }
        public byte[] Icon { get; set; }
        public byte[] Screenshot { get; set; }
    }

    [DataContract]
    class ProjectsList
    {
        [DataMember]
        public List<ProjectData> Projects { get; set; }
    }
    class OpenProject : VMBase
    {
        static OpenProject()
        {
            try
            {
                if (!Directory.Exists(_appData))
                    Directory.CreateDirectory(_appData);
                _projectDataPath = $@"{_appData}ProjectData.xml";
                Projects = new ReadOnlyObservableCollection<ProjectData>(_projects);

                ReadProjectData();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }
        public static Project Open(ProjectData projectData)
        {
            ReadProjectData();
            var project = _projects.FirstOrDefault(x => x.FullPath == projectData.FullPath);
            if (project != null)
            {
                project.LastOpened = DateTime.Now;
            }
            else
            {
                project = projectData;
                project.LastOpened = DateTime.Now;
                _projects.Add(project);
            }
            WrirteProjectData();
            return Project.Load(project.FullPath);
        }

        static public ReadOnlyObservableCollection<ProjectData> Projects { get; }

        private static void ReadProjectData()
        {
            if (File.Exists(_projectDataPath))
            {
                var projects = Utils.Serializer.Deserialize<ProjectsList>(_projectDataPath).Projects.OrderByDescending(x => x.LastOpened);
                _projects.Clear();
                foreach (var project in projects)
                {
                    if (File.Exists(project.FullPath))
                    {
                        project.Icon = File.ReadAllBytes($@"{project.ProjectPath}\.engine\Icon.png");
                        project.Screenshot = File.ReadAllBytes($@"{project.ProjectPath}\.engine\Screenshot.png");
                        _projects.Add(project);
                    }
                }
            }
        }
        private static void WrirteProjectData()
        {
            var projects = _projects.OrderBy(x => x.LastOpened).ToList();
            Utils.Serializer.Serialize(new ProjectsList() { Projects = projects }, _projectDataPath);
        }
        static private ObservableCollection<ProjectData> _projects = new ObservableCollection<ProjectData>();
        private static readonly string _appData = $@"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}\maxamula\";
        private static readonly string _projectDataPath;
    }
}
