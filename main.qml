import QtQuick 2.0
import ProcessLib 1.0
//import 'ProcList.js' as ProcList

Rectangle {
    id: main
    width: 360
    height: 360
    color: "#000000"

    property int legendWidth: 400

    ProcessList {
        id: proc;
    }

    Component {
        id: legend
        Item {
            height: 20
            x: legends.x
            width: legends.width
            Rectangle {
                id: box
                anchors.verticalCenter: parent.verticalCenter
                width: 10
                height: 10
                color: "#FFFFFF"
            }
            Text {
                id: label
                anchors.verticalCenter: box.verticalCenter
                anchors.left: box.right
                anchors.leftMargin: 5
                color: "#FFFFFF"
                font.family: "Helvetica"
                font.pointSize: 16
                text: "green text"
            }
            function init(color, text) {
                box.color = color;
                label.text = text;
            }
        }
    }

    Timer {
        interval: 500;
        running: true;
        repeat: true
        onTriggered: {
            proc.update();
            graph.requestPaint();
            // update labels
            legends.update(proc.data());
        }
    }

    Canvas {
        id: graph
        antialiasing: true
        anchors.top: parent.top
        anchors.right: legends.left
        renderTarget: Canvas.Image
        renderStrategy: Canvas.Immediate
        width: parent.width - legends.width
        height: parent.height

        onPaint: {

            var w = graph.width;
            var h = graph.height;

            var ctx = graph.getContext('2d');

            ctx.save();
            ctx.fillStyle = '#000';
            ctx.strokeStyle = ctx.fillStyle;
            ctx.clearRect(0, 0, w, h);
            //ctx.fillRect(0, 0, w, h);

            var data = proc.data();

            circleGraph(ctx, w / 2, h / 2,
                        Math.min(w, h) / 2 - 10,
                        data);

            ctx.fill();
            ctx.stroke();

            ctx.restore();
        }

        function fun(ctx, x, y, radius, startAngle, endAngle, anticlockwise) {
            ctx.beginPath();
            ctx.moveTo(x, y);
            ctx.arc(x, y, radius, startAngle, endAngle, anticlockwise);
            ctx.closePath();
            ctx.fill();
            ctx.stroke();
        }

        function circleGraph(ctx, cx, cy, r, data) {
            // data = [25, 50, 11];
            // ↑ソートされていることが望ましい

            ctx.fillStyle = '#fff';
            ctx.strokeStyle = '#000';
            ctx.lineWidth = '4.0';

            var i;
            // 与えられたデータの合計を計算
            var dataTotal = 0;
            for (i = 0; i < data.length; i++) {
                dataTotal += data[i].memory;
            }
            // 描画
            var dataCur = 0;
            var rgb;
            for (i = 0; i < data.length; i++) {
                var dataNext = dataCur + data[i].memory;
                var curAngle  = (mod(dataCur / dataTotal * 360 + 270, 360) / 180) * Math.PI;
                var nextAngle = (mod(dataNext / dataTotal * 360 + 270, 360) / 180) * Math.PI;
                // 円弧を描画
                rgb = hsv2rgb(Math.floor(dataCur / dataTotal * 360), 94, 256);
                ctx.fillStyle = rgb2htmlrgb(rgb.r, rgb.g, rgb.b);
                fun(ctx, cx, cy, r, curAngle, nextAngle, false);
                //
                dataCur = dataNext;
            }
        }
    }

    Repeater {
        id: legends
        model:ListModel{}
        anchors.top: parent.top
        x: parent.width - main.legendWidth
        width: main.legendWidth
        height: parent.height

        Loader {
            sourceComponent: srcObject
            onLoaded: {
                //item.visible = true;
                item.y = (legends.count - 1) * item.height;
                item.init(itemColor, itemLabel);
            }
        }

        function update(data) {
            // 追加済みのラベルをクリア
            legends.model.clear();
            // 与えられたデータの合計を計算
            var i;
            var dataTotal = 0;
            for (i = 0; i < data.length; i++) {
                dataTotal += data[i].memory;
            }
            // ラベルを追加
            var dataCur = 0;
            var rgb;
            for (i = 0; i < data.length; i++) {
                var dataNext = dataCur + data[i].memory;
                rgb = hsv2rgb(Math.floor(dataCur / dataTotal * 360), 94, 256);
                //
//                legends.itemColor = rgb2htmlrgb(rgb.r, rgb.g, rgb.b);
//                legends.itemLabel = data[i].label;
                legends.model.append({
                                         "srcObject": legend,
                                         "itemColor": rgb2htmlrgb(rgb.r, rgb.g, rgb.b),
                                         "itemLabel": data[i].name,
                                     });
                //
                y += 20;
                dataCur = dataNext;
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    function hsv2rgb(h, s, v) {
        var r, g, b;
        for (; h < 0; h += 360);
        h = h % 360;
        if (0 == s) {
          v = Math.round(v);
          return {'r': v, 'g': v, 'b': v};
        }
        s = s / 255;
        var i = Math.floor(h / 60) % 6,
            f = (h / 60) - i,
            p = v * (1 - s),
            q = v * (1 - f * s),
            t = v * (1 - (1 - f) * s)
        switch (i) {
          case 0: r = v;  g = t;  b = p;  break;
          case 1: r = q;  g = v;  b = p;  break;
          case 2: r = p;  g = v;  b = t;  break;
          case 3: r = p;  g = q;  b = v;  break;
          case 4: r = t;  g = p;  b = v;  break;
          case 5: r = v;  g = p;  b = q;  break;
        }
        return {
            'r': Math.min(Math.round(r), 255),
            'g': Math.min(Math.round(g), 255),
            'b': Math.min(Math.round(b), 255) };
    }

    function rgb2htmlrgb(r, g, b) {
        return '#'
                + ('00' + r.toString(16)).substr(-2, 2)
                + ('00' + g.toString(16)).substr(-2, 2)
                + ('00' + b.toString(16)).substr(-2, 2)
        ;
    }

    function mod(a, b) {
        return a - Math.floor(a / b) * b;
    }
}

