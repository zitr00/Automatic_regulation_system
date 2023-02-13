using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ProiecteSincretic
{
    public partial class MainApp : Form
    {
        public SerialPort myPort;
        string in_data;
        int timp=0;
        public MainApp()
        {
            InitializeComponent();
            init();
        }

        void init()
        {

            var objChart = chart1.ChartAreas[0];
            objChart.AxisX.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
            objChart.AxisX.Minimum = 0;
            objChart.AxisX.Maximum = 300;

            objChart.AxisY.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
            objChart.AxisY.Minimum = 0;
            objChart.AxisY.Maximum = 45;

            chart1.Series.Clear();
            chart1.Series.Add("PS2022");
            chart1.Series["PS2022"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            chart1.Series["PS2022"].BorderColor = Color.White;
        }

        private void MyPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            
            in_data = myPort.ReadLine();
            this.Invoke(new EventHandler(displaydata_event)); 
            
        }

        private void displaydata_event(object sender, EventArgs e)
        {
            timp++;
            chart1.Series["PS2022"].Points.AddXY(timp, float.Parse(in_data));
        }


        private void MainApp_Load(object sender, EventArgs e)
        {
            myPort = new SerialPort();
            myPort.BaudRate = 9600;
            myPort.PortName = "COM3";
            myPort.Parity = Parity.None;
            myPort.DataBits = 8;
            myPort.StopBits = StopBits.One;
            myPort.DataReceived += MyPort_DataReceived;
            myPort.Open();
        }
    }
}
