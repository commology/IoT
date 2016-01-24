$(function() {
  drawChart();
});

function updateData(chart, option) {
  $.ajax({
    url: '/IoT/data',
    cache: false
  })
  .done(function(datajson) {
    var timestr = [];
    datajson.timestring.forEach(function (element, index, array) {
      var s = element.substring(11, 19);
      timestr.push(s);
    });
    option.xAxis[0].data = timestr;
    option.xAxis[1].data = timestr;
    option.series[0].data = datajson.temperature;
    option.series[1].data = datajson.humidity;
    chart.setOption(option);
  });
}

function drawChart() {

  var mainChart = echarts.init(document.getElementById('eChart'));

  var option = {
    title: {
      text: 'Simple Realtime Monitoring',
      x: 'center'
    },
    tooltip: {
      trigger: 'axis',
      formatter: function (params) {
        var unit = '';
        if (params[0].seriesName == 'Temp.')
          unit = 'C';
        if (params[0].seriesName == 'R.H.')
          unit = '%';
        return params[0].name + '<br/>' +
               params[0].seriesName + ' : ' + params[0].value + unit;
      },
      axisPointer: {
        animation: false
      }
    },
    legend: {
      data: ['Temp.', 'R.H.'],
      x: 'left',
      left: 100
    },
    grid: [
      {
        show: true,
        containLabel: true,
        height: '35%'
      },
      {
        show: true,
        containLabel: true,
        top: '55%',
        height: '35%'
      }
    ],
    xAxis: [
      {
        gridIndex: 0,
        type: 'category',
        boundaryGap: false,
        axisLabel: {
          interval: 5
        },
        axisLine: {
          onZero: true
        },
        data: []
      },
      {
        gridIndex: 1,
        type: 'category',
        boundaryGap: false,
        axisLabel: {
          interval: 5
        },
        axisLine: {
          onZero: true
        },
        data: []
      }
    ],
    yAxis: [
      {
        gridIndex: 0,
        name: 'Temp.',
        type: 'value',
        interval: 5
      },
      {
        gridIndex: 1,
        name: 'R.H.',
        type: 'value',
        interval: 10,
        min: 0,
        max: 100
      }
    ],
    series: [
      {
        name: 'Temp.',
        type: 'line',
        xAxisIndex: 0,
        yAxisIndex: 0,
        hoverAnimation: false,
        itemStyle: {
          normal: {
            color: 'Orange'
          }
        },
        lineStyle: {
          normal: {
            color: 'Orange'
          }
        },
        areaStyle: {
          normal: {
            color: 'Orange'
          }
        },
        data: []
      },
      {
        name: 'R.H.',
        type: 'line',
        xAxisIndex: 1,
        yAxisIndex: 1,
        itemStyle: {
          normal: {
            color: 'SkyBlue'
          }
        },
        lineStyle: {
          normal: {
            color: 'SkyBlue'
          }
        },
        data: []
      }
    ]
  };

  updateData(mainChart, option);

  timerTicket = setInterval(function () {
    updateData(mainChart, option);
  }, 5000);
}

