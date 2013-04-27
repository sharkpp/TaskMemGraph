import QtQuick 2.0
import 'ProcList.js' as ProcList

Rectangle {
    width: 360
    height: 360
    /*Text {
        anchors.centerIn: parent.top
        font.family: "Helvetica"
        font.pointSize: 24
        text: "Hello World"
    }
    Text {
        anchors.centerIn: parent.top
        color: "#00FF00"
        font.family: "Helvetica"
        font.pointSize: 16
        text: "green text"
    }*/
    Timer {
        interval: 500;
        running: true;
        repeat: true
        onTriggered: {
            ProcList.updateData();
            canvas.requestPaint();
        }
    }
    Canvas {
        id: canvas
        antialiasing: true
        anchors.fill: parent
        renderTarget:Canvas.Image
        renderStrategy: Canvas.Immediate

        onPaint: {

            var w = canvas.width;
            var h = canvas.height;

            var ctx = canvas.getContext('2d');

            ctx.save();
            ctx.fillStyle = '#000';
            ctx.strokeStyle = ctx.fillStyle;
//            ctx.clearRect(0, 0, w, h);
            ctx.fillRect(0, 0, w, h);

          //  ctx.beginPath();

            var data = ProcList.getData();

            circleGraph(ctx, (w - 100) / 2, h / 2,
                        Math.min(w - 100, h) / 2 - 10,
                        data);
            dataList(ctx, w - 100, 10, data);

            ctx.fill();
            ctx.stroke();

            ctx.restore();
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
                dataTotal += data[i].value;
            }
            // 描画
            var dataCur = 0;
            var rgb;
            for (i = 0; i < data.length; i++) {
                var dataNext = dataCur + data[i].value;
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

        function dataList(ctx, x, y, data) {

            ctx.fillStyle = '#fff';
            ctx.strokeStyle = '#000';

            var i;
            // 与えられたデータの合計を計算
            var dataTotal = 0;
            for (i = 0; i < data.length; i++) {
                dataTotal += data[i].value;
            }
            var box = 15;
            var margin = 5;
            // 描画
            var dataCur = 0;
            var rgb;
            for (i = 0; i < data.length; i++) {
                var dataNext = dataCur + data[i].value;
                rgb = hsv2rgb(Math.floor(dataCur / dataTotal * 360), 94, 256);
                ctx.fillStyle = rgb2htmlrgb(rgb.r, rgb.g, rgb.b);
                ctx.lineWidth = '0.0';
                ctx.beginPath();
                ctx.rect(x, y, box, box);
                ctx.closePath();
                ctx.fill();
                // 文字を描画
                ctx.font = '18px';
                ctx.lineWidth = '1.0';
                ctx.strokeStyle = '#fff';
                ctx.beginPath();
                ctx.fillText(data[i].label, x + box + margin, y + 15);
                ctx.closePath();
                ctx.stroke();
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
}

