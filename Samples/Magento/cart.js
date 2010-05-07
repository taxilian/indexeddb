var connectionName = "IndexedDBMagentoDemo";
var databaseName = "IndexedDBMagentoDemoDatabase";
var indexName = "QuantityIndex"
var connection = undefined;
var objectStore = undefined;

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
			objectStore = connection.openObjectStore(databaseName);
		}
		catch (e) {
			handleError(e, "while opening the shopping cart object store");
			return false;
		}

		// Open our index in an equally lazy manner.
		try {
			indexByName = objectStore.openIndex(indexName);
		}
		catch (e) {
			handleError(e, "while opening the shopping cart index");
			return false;
		}
	}
	return true;
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

function removeFromCart(name) {
	// A really sloppy way to empty the index of keys
	try {
		while (indexByName.get(name) != undefined)
			indexByName.remove(name);
	}
	catch (e) {
	}

	// Absurdly flashy and unnecessary method of hiding the TR.  
	$("table#shopping-cart-table tr#clone" + name.replace(/[ :-]/g, "_") + " td").slideUp();
	$("table#shopping-cart-table tr#clone" + name.replace(/[ :-]/g, "_")).hode();
}

// Called when our document is ready
$(document).ready(function() {
	// Open our connection
	if (!openConnection())
		return;

	// We're going to keep a hash of items we've touched.  Not very efficient
	var items = [];
	var itemCount = 0;

	// Open a cursor over our object store
	try {
		var cursor = objectStore.openCursor();
	}
	catch (e) {
		return;
	} // No objects in our object store
	do {
		// If we haven't visited this item yet...
		if (!items[cursor.value.name]) {
			// Make a clone of our cart row, set its id and make it visible
			var clonedRow = $("table#shopping-cart-table tr#CloneRow").clone();
			$(clonedRow).attr("id", "clone" + cursor.value.name.replace(/[ :-]/g, "_"));
			$(clonedRow).attr("style", "display: auto");

			// Set the attributes for our row (name, price, total, image, and removal script
			$("a.btn-remove2", clonedRow).attr("href", "javascript: removeFromCart('" + cursor.value.name + "');");
			$("h2.product-name", clonedRow).text(cursor.value.name);
			$("span.price", clonedRow).text("$" + cursor.value.price + ".00");
			$("span.price", clonedRow).text("$" + cursor.value.price + ".00");
			$("td img.product", clonedRow).attr("src", cursor.value.image);

			// Use our index to get a count of the items that have this name
			var range = document.db.IDBKeyRange.only(cursor.value.name)
			try {
				var countCursor = indexByName.openCursor(range);
				itemCount = countCursor.count;
				// Done with the cursor, close it (this is not supported by the spec!)
				countCursor.close();
			}
			catch (e) {
				// Yes, this is an error and we're totally punting.  Don't tell anyone!
				itemCount = 1;
			}

			// Use our count cursor to get total quantity
			$("td input.qty", clonedRow).attr("value", (itemCount + 1));
			// Quantity * line price = line total
			$("td.last span.price", clonedRow).text("$" + (itemCount + 1) * cursor.value.price + ".00");

			// Finished with our row, add it to the page
			$("table#shopping-cart-table").append(clonedRow);

			// Mark the item as having been handled
			items[cursor.value.name] = 1;
		}
	} while (cursor["continue"]());

	cursor.close();
});