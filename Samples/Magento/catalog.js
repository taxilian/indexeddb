var connectionName = "IndexedDBMagentoDemo";
var databaseName = "IndexedDBMagentoDemoDatabase";
var dialogStoreName = "IndexedDBMagentoDemoDialogDatabase";
var indexName = "QuantityIndex"
var connection = undefined;
var objectStore = undefined;
var dialogStore = undefined;

function openConnection() {
	// Ensure we don't open our connection twice
	if (connection == undefined) {
		// Open our connection
		try {
			connection = document.getElementById("db").indexedDB.open(connectionName, "A demonstration database for the Magento e-commerce pages.");
		}
		catch (e) {
			handleError(e, "while opening the Indexed Database connection");
			return false;
		}

		// Open our object store
		try {
			objectStore = connection.createObjectStore(databaseName, undefined, true);
		}
		catch (e) {
			// This is a really lazy way of determining if the object store exists.  Please don't try at home.
			try {
				objectStore = connection.openObjectStore(databaseName);
			}
			catch (e) {
				handleError(e, "while opening the shopping cart object store");
				return false;
			}
		}

		// Open our dialog store
		try {
			dialogStore = connection.createObjectStore(dialogStoreName, undefined, false);
		}
		catch (e) {
			// This is a really lazy way of determining if the object store exists.  Please don't try at home.
			try {
				dialogStore = connection.openObjectStore(dialogStoreName);
			}
			catch (e) {
				handleError(e, "while opening the dialog object store");
				return false;
			}
		}

		// Open our index in an equally lazy manner.
		try {
			indexByName = objectStore.createIndex(indexName, "name", false);
		}
		catch (e) {
			try {
				indexByName = objectStore.openIndex(indexName);
			}
			catch (e) {
				handleError(e, "while opening the shopping cart index");
				return false;
			}
		}
	}
	return true;
}


function addToCart(name, price, color, image) {
	// Add the item to our database... that's it
	// (Note that it would be more efficient to just increment the quantity... but that's less fun)
	try {
		objectStore.put(
		{
			name: name,
			price: price,
			color: color,
			image: image
		});
	}
	catch (e) {
		handleError(e, "adding the item to the cart database");
		return;
	}

	// Off to the cart page
	window.location = "cart.html";
}

// Handle an error by display a separate dialog
function handleError(exception, source) {
	// Close the info dialog, in case it's open
	$("#dialog").dialog("close");

	// Fill the dialog with some helpful text
	$("span#errorText").text(source);
	$("span#exceptionText").text(exception.hasOwnProperty("description") ? exception.description : exception);
	
	// Show the dialog
	$("#errorDialog").dialog(
    {
    	modal: true,
    	width: 500,
    	close: function() {
    		return false;
    	}
    });
}

// Called when our document is ready
$(document).ready(function() {
	// Open a connection, put an object, and redirect to cart
	if (openConnection())
		try {
			dialogStore.get("hasViewedDialog");
		}
	catch (e) {
		$("#dialog").dialog(
        {
        	width: 560,
        	position: "center",
        	modal: true,
        	close: function() {
        		dialogStore.put(true, "hasViewedDialog");
        	}
        });
	}
});