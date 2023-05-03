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
using System.Windows.Media.Media3D;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Editor.Utils
{
    /// <summary>
    /// Логика взаимодействия для GeometryView.xaml
    /// </summary>
    public partial class GeometryView : UserControl
    {
        public void SetGeometry(int index = -1)
        {
            if (!(DataContext is MeshRenderer vm)) return;
            if (vm.Meshes.Any() && viewport.Children.Count == 2)
            {
                viewport.Children.RemoveAt(1);
            }

            var meshIndex = 0;
            var modelGroup = new Model3DGroup();
            foreach (var mesh in vm.Meshes)
            {
                if (index != -1 && meshIndex != index)
                {
                    ++meshIndex;
                    continue;
                }

                var mesh3D = new MeshGeometry3D()
                {
                    Positions = mesh.Positions,
                    Normals = mesh.Normals,
                    TriangleIndices = mesh.Indices,
                    TextureCoordinates = mesh.UVs
                };

                var diffuse = new DiffuseMaterial(mesh.Diffuse);
                var specular = new SpecularMaterial(mesh.Specular, 50);
                var matGroup = new MaterialGroup();
                matGroup.Children.Add(specular);
                matGroup.Children.Add(diffuse);

                var model = new GeometryModel3D(mesh3D, matGroup);
                modelGroup.Children.Add(model);

                var binding = new Binding(nameof(mesh.Diffuse)) { Source = mesh };
                BindingOperations.SetBinding(diffuse, DiffuseMaterial.BrushProperty, binding);

                if (meshIndex == index) break;
            }

            var visual = new ModelVisual3D() { Content = modelGroup };
            viewport.Children.Add(visual);
        }
        public GeometryView()
        {
            InitializeComponent();
            DataContextChanged += (s, e) => SetGeometry();
        }

        Point _clickPos;
        Point3D _oldCamPos;
        Point3D _oldCamTarg;
        bool _captureLeft = false;
        bool _captureRight = false;
        private void Grid_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            var vm = DataContext as MeshRenderer;
            _clickPos = e.GetPosition(this);
            _oldCamPos = new Point3D(vm.CameraPos.X, vm.CameraPos.Y, vm.CameraPos.Z);
            _captureLeft = true;
            Mouse.Capture(sender as UIElement);
        }

        private void Grid_MouseMove(object sender, MouseEventArgs e)
        {
            if (!_captureLeft && !_captureRight) return;
            
            var pos = e.GetPosition(this);
            var delta = pos - _clickPos;

            if(_captureLeft && !_captureRight)
            {
                MoveCamera(delta.X, delta.Y, 0);
            }
            else if (!_captureLeft && _captureRight)
            {
                var vm = DataContext as MeshRenderer;
                var yoffset = delta.Y * 0.01;// * Math.Sqrt(campos.X * campos.X + campos.Y * campos.Y);
                vm.CameraTarg = new Point3D(_oldCamTarg.X, _oldCamTarg.Y + yoffset, _oldCamTarg.Z);
            }
        }
        public static T Clamp<T>(T value, T min, T max) where T : IComparable<T>
        {
            if (value.CompareTo(min) < 0)
            {
                return min;
            }
            else if (value.CompareTo(max) > 0)
            {
                return max;
            }
            else
            {
                return value;
            }
        }

        private void MoveCamera(double dx, double dy, int dz)
        {
            var vm = DataContext as MeshRenderer;
            var v = new Vector3D(_oldCamPos.X, _oldCamPos.Y, _oldCamPos.Z);

            var r = v.Length;
            var theta = Math.Acos(v.Y / r);
            var phi = Math.Atan2(-v.Z, v.X);

            theta -= dy * 0.01;
            phi -= dx * 0.01;
            r *= 1.0 - 0.1 * dz;

            theta = Clamp(theta, 0.0001, Math.PI - 0.0001);

            v.X = r * Math.Sin(theta) * Math.Cos(phi);
            v.Z = -r * Math.Sin(theta) * Math.Sin(phi);
            v.Y = r * Math.Cos(theta);
            vm.CameraPos = new Point3D(v.X, v.Y, v.Z);
        }

        private void Grid_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _captureLeft = false;
            if(!_captureRight) Mouse.Capture(null);
        }

        private void Grid_MouseWheel(object sender, MouseWheelEventArgs e)
        {

        }

        private void Grid_MouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            var vm = DataContext as MeshRenderer;
            _clickPos = e.GetPosition(this);
            _oldCamTarg = new Point3D(vm.CameraTarg.X, vm.CameraTarg.Y, vm.CameraTarg.Z);
            _captureRight = true;
            Mouse.Capture(sender as UIElement);
        }

        private void Grid_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            _captureRight = false;
            if (!_captureLeft) Mouse.Capture(null);
        }

        private static readonly GeometryView s_geomertyView = new GeometryView { Background = Brushes.Aqua }; 
        public static BitmapSource RenderToBitmap(MeshRenderer mesh, int width, int height)
        {
            var bmp = new RenderTargetBitmap(width, height, 96, 96, PixelFormats.Default);
            s_geomertyView.DataContext = mesh;
            s_geomertyView.Width = width;
            s_geomertyView.Height = height;
            s_geomertyView.Measure(new System.Windows.Size(width, height));
            s_geomertyView.Arrange(new Rect(new System.Windows.Size(width, height)));
            s_geomertyView.UpdateLayout();
            bmp.Render(s_geomertyView);
            return bmp;
        }
    }
}
