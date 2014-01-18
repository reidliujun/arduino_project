
/*
 * GET home page.
 */

exports.index = function(db){
	return function(req, res){
	  var collection = db.get('statistics');
	  var now = new Date();
	  var date = null;
	 
	  if (req.query.date) {
	  	try {
	  		var c = req.query.date.split("-");
	  		date = new Date(c[0], c[1]-1, c[2]);
	  		
	  	} catch(err) {
	  	}
	  }
	  if (date == null) {
	  	date = new Date(now.getYear()+1900, now.getMonth(), now.getDate());
	  }
	  
	  var start = date;
	  var end = new Date(start.getYear()+1900, start.getMonth(), start.getDate() + 1);
	  collection.find({},{created: 1, value: 1}, function(e,docs){

	  		var arr = [];
	  		var lastBrightness = 0;
	  		if (docs.length > 0) {
		  		lastBrightness = docs[docs.length - 1].value;
		  		
		  		for (var i = 0, n = docs.length; i < n; i++) {
		  			if (start <= docs[i].created && docs[i].created < end)
		  				arr.push({"created" : docs[i].created, "value": docs[i].value});
		  		}
	  		}
	  		var nextDay = new Date(start.getFullYear(), start.getMonth(), start.getDate()+1);
	  		var prevDay = new Date(start.getFullYear(), start.getMonth(), start.getDate()-1);
	  	  	res.render('index', { 
			  	"title": 'Arduino Remote Light Control',
			  	"led_status": lastBrightness < 600 ? 'ON' : 'OFF',
			  	"brightness": lastBrightness,
			  	data: arr,
			  	len: docs.length,
			  	year: start.getYear() + 1900,
			  	month: start.getMonth() + 1,
			  	day: start.getDate(),
			  	"nextDay": (nextDay.getYear()+1900) + "-" + (nextDay.getMonth()+1) + "-" + nextDay.getDate(),
			  	"prevDay": (prevDay.getYear()+1900) + "-" + (prevDay.getMonth()+1) + "-" + prevDay.getDate()
	      	});
	  });
	  
	}
};

exports.helloworld = function(req, res){
  res.render('helloworld', { title: 'Hello, World!' });
};

exports.userlist = function(db, fayeClient) {
    return function(req, res) {
    	res.render('helloworld', {title: 'Sender'});
    	
        // var collection = db.get('usercollection');
        // collection.find({},{},function(e,docs){
        //     res.render('userlist', {
        //         "userlist" : docs
        //     });
        // });
    };
};

exports.newuser = function(req, res){
  res.render('newuser', { title: 'Add New User' });
};

exports.adduser = function(db) {
    return function(req, res) {

        // Get our form values. These rely on the "name" attributes
        var userName = req.body.username;
        var userEmail = req.body.useremail;

        // Set our collection
        var collection = db.get('usercollection');

        // Submit to the DB
        collection.insert({
            "username" : userName,
            "email" : userEmail
        }, function (err, doc) {
            if (err) {
                // If it failed, return error
                res.send("There was a problem adding the information to the database.");
            }
            else {
                // If it worked, forward to success page
                res.redirect("userlist");
                // And set the header so the address bar doesn't still say /adduser
                res.location("userlist");
            }
        });

    }
}