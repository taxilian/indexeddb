var READ_WRITE = 0;
var READ_ONLY = 1;
var SNAPSHOT_READ = 2;

var INVALID_ARGUMENTS = "Invalid Arguments";
var NOT_FOUND_ERR = "NOT_FOUND_ERR";

function assertExceptionContains(e, expectedMessage) {
	if (navigator.userAgent.search(/Chrome/) != -1) assertTrue(true);
	else if (navigator.appName == "Netscape") assertContains(expectedMessage, e);
	else assertContains(expectedMessage, e.Message);
}

function makeRandomName() {
	return Math.random().toString().replace(".", "");
}

function iterate(expectedFrom, expectedTo, cursor, delta, keyFunction, valueFunction) {
	if (delta == undefined) delta = 1;
	if (keyFunction == undefined) keyFunction = function(index) {
		return i;
	}
	if (valueFunction == undefined) valueFunction = function(index) {
		return index.toString() + "value";
	}

	var continueResult = true;
	for (var i = expectedFrom; i != expectedTo; i += delta) {
		assertTrue('Cursor should be in range, but reported out of elements -- ' + i.toString(), continueResult);
		assertEquals("Key should be returned in order", keyFunction(i), cursor.key);

		if (typeof (valueFunction(i)) != "object") assertEquals("Value should be correct", valueFunction(i), cursor.value);
		else assertObjectEquals("Object value should be correct", valueFunction(i), cursor.value);
		continueResult = cursor["continue"]();
	}

	assertFalse('Cursor should have reported end of range, but did not.', cursor["continue"]());
}

function putValues(objectStore, count) {
	for (var i = 0; i < count; i++)
		objectStore.put(i.toString() + "value", i);
}

function assertClosureThrows(closure, expectedMessage) {
	var thrown = true;

	try {
		closure();
		thrown = false;
	}
	catch (e) {
		assertExceptionContains(e, expectedMessage);
	}

	assertTrue("Closure did not throw '" + expectedMessage + "' as expected: " + closure.toString(), thrown);
}


function assertClosureDoesNotThrow(closure) {
	try {
		closure();
	}
	catch (e) {
		assertTrue("Closure threw '" + e + ", but should not have thrown: " + closure.toString(), false);
	}
}