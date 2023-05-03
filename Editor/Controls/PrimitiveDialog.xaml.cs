using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Editor.Content;
using Editor.Utils;
using Microsoft.Win32;

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
            thread = new Thread(_UpdatePrimitive);
            Loaded += (s, e) => { _UpdatePrimitive(); };
        }

        private void OnTypeChanged(object sender, SelectionChangedEventArgs e)
        {
            _UpdatePrimitive();
        }

        private Thread thread;

        private async void PreviewSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
             _UpdatePrimitive();
        }
        private void NumBox_ValueChanged(object sender, RoutedEventArgs e) => _UpdatePrimitive();

        private void SaveClick(object sender, RoutedEventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog() { InitialDirectory = Project.Project.Current.ContentPath, Filter = "Asset file (*.asset)|*.asset"};
            if(sfd.ShowDialog() == true)
            {
                var asset = (DataContext as IAssetEditor).Asset;
                asset.Save(sfd.FileName);
                this.Close();
            }
        }

        private void _UpdatePrimitive()
        {
            if (!IsInitialized) return;
            var type = (PrimitiveType)cbType.SelectedItem;
            //var type = (PrimitiveType)cbType.SelectedItem;
            var desc = new PrimitiveDesc();
            desc.type = (uint)type;
            switch (type)
            {
                case PrimitiveType.Plane:
                    {
                        desc.slicesX = (uint)planeSliceX.Value;
                        desc.slicesY = (uint)planeSliceY.Value;
                        float.TryParse(planeSizeX.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.width);
                        float.TryParse(planeSizeY.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.height);
                        desc.LOD = 0;
                    break;
                    }
                case PrimitiveType.Box:
                    {
                        desc.slicesX = (uint)boxSliceX.Value;
                        desc.slicesY = (uint)boxSliceY.Value;
                        desc.slicesZ = (uint)boxSliceZ.Value;
                        float.TryParse(boxSizeX.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.width);
                        float.TryParse(boxSizeY.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.height);
                        float.TryParse(boxSizeZ.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.depth);
                        break;
                    }
                    case PrimitiveType.Sphere: 
                    {
                        desc.slicesX = (uint)sphereSlice.Value;
                        desc.slicesY = (uint)sphereSegments.Value;
                        float.TryParse(sphereRad.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.width);
                        break;
                    }
                    case PrimitiveType.IcoSphere:
                    {
                        desc.slicesX = (uint)icoSlice.Value;
                        float.TryParse(icoRad.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.width);
                        break;
                    }
                    case PrimitiveType.Torus:
                    {
                        desc.slicesX = (uint)torusSlice.Value;
                        desc.slicesY = (uint)torusSegments.Value;
                        float.TryParse(torusInnerRad.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.width);
                        float.TryParse(torusOuterRad.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.height);
                        break;
                    }
                    case PrimitiveType.Cone:
                    {
                        desc.slicesX = (uint)cylSlice.Value;
                        desc.slicesY = (uint)cylSegments.Value;
                        float.TryParse(cylSizeX.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.width);
                        float.TryParse(cylSizeY.Value, NumberStyles.Float, CultureInfo.InvariantCulture, out desc.height);
                        break;
                    }
            }
            var parameters = new ImportParams();
            parameters.bRH = 1;
            parameters.bGenerateNormals = 1;
            parameters.bGenerateTangents = 1;
            parameters.smoothingAngle = _smoothingAngle; // 37.82967

            var data = new GeometryData();
            CManager.CreatePrimitive(desc, parameters, data);

            byte[] buffer = new byte[data.size];
            Marshal.Copy(data.data, buffer, 0, (int)data.size);

            var geometry = new Content.Geometry();
            geometry.FromRawData(buffer);
            (DataContext as Utils.GeometryEditor).SetAsset(geometry);
        }

        private float _smoothingAngle = 60.0f;
        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            float.TryParse((string)((sender as RadioButton).Tag), NumberStyles.Float, CultureInfo.InvariantCulture, out _smoothingAngle);
            _UpdatePrimitive();
        }
    }
}
