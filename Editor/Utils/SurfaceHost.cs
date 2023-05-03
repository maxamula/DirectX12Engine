using Editor.GameProject;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Interop;

namespace Editor.Utils
{
    class SurfaceHost : HwndHost
    {
        public SurfaceHost(ushort width, ushort height)
        {
            _width = width;
            _height = height;
        }
        public bool CanResize { get; set; } = true;
        public void ResizeSwapChain()
        {
            if(swapchainSize != controlSize)
            {
                swapchainSize = controlSize;
                Engine.Viewport.Resize((ushort)controlSize.Width, (ushort)controlSize.Height);
            }    
        }

        public delegate void WindowCreatedEventHandler();
        public event WindowCreatedEventHandler WindowCreated;

        Size controlSize = Size.Empty;
        Size swapchainSize = Size.Empty;
        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            base.OnRenderSizeChanged(sizeInfo);
            controlSize = sizeInfo.NewSize;
            if(CanResize)
            {
                ResizeSwapChain();
            }
        }
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            try
            {
                var hwnd = Engine.Viewport.SetParent(hwndParent.Handle);
                WindowCreated?.Invoke();
                return new HandleRef(this, hwnd);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                this.Dispose();
                return new HandleRef(this, IntPtr.Zero);
            }
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            
        }

        // Intermediate data
        private ushort _width;
        private ushort _height;
    }
}
