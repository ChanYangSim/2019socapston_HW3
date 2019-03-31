var express = require('express')
var app = express()
var fs = require('fs');
var mysql = require('mysql');
var db_config = require('./log/account.json');
var connection = mysql.createConnection({
    host: db_config.host,
    user: db_config.user,
    password: db_config.password,
    database: db_config.database
})
connection.connect();


app.get('/graph', function (req, res) {
    console.log('got app.get(graph)');
    var html = fs.readFile('./graph_t.html', function (err, html) {
    html = " "+ html
    console.log('read file');

    var qstr = 'select * from sensors ';
    connection.query(qstr, function(err, rows, cols) {
            if (err) throw err;

        var data = ""
        var comma = ""
        var fir = ""
        var las = ""
        for (var i=0; i< rows.length; i++) {
                r = rows[i];
                rmon = String(Number(r.time.getMonth()))
                if( Number(rmon)<10) rmon = "0"+rmon;
                data += comma + "[new Date(" + r.time.getFullYear() +","
                      + rmon +","+ r.time.getDate() +","
                      + r.time.getHours() +"," +r.time.getMinutes()
                      + "," + r.time.getSeconds()+ ")"  + ","
                      + r.value +"]";
                comma = ",";
                if(i==0){
                        fir += r.time.getFullYear()+"-"+Number(r.time.getMonth()+1)+"-"+
                                r.time.getDate()+" "+r.time.getHours()+":"+
                                r.time.getMinutes()+":"+r.time.getSeconds() ;
                }
                if(i==rows.length-1){
                        las += r.time.getFullYear()+"-"+Number(r.time.getMonth()+1)+"-"+
                                r.time.getDate()+" "+r.time.getHours()+":"+
                                r.time.getMinutes()+":"+r.time.getSeconds() ;
                }

      }
      var header = "data.addColumn('datetime', 'Date/Time');"
      header += "data.addColumn('number', 'Temp');"
      html = html.replace("<%HEADER%>", header);
      html = html.replace("<%DATA%>", data);
      html = html.replace("<%FIRST%>",fir);
      html = html.replace("<%LAST%>",las);
      res.writeHeader(200, {"Content-Type": "text/html"});
      res.write(html);
      res.end();
    });
  });
})

var server = app.listen(8082, function () {
  var host = server.address().address
  var port = server.address().port
  console.log('listening at http://%s:%s', host, port)
});
