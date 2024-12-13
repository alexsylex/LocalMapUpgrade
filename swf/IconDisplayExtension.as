import LocalMap.ExtraMapMarker;

var extraMarkerContainer:MovieClip = this;
var extraMarkers:Array = new Array();

var CREATE_ICONTYPE:Number = 1;
var CREATE_STRIDE:Number = 3;

_parent.IconDisplay.ExtraMarkerData = new Array();

/* API */
_parent.IconDisplay.PostCreateMarkers = function(a_showEnemyMarkers:Boolean, a_showHostileMarkers:Boolean,
												 a_showGuardMarkers:Boolean, a_showDeadMarkers:Boolean,
												 a_showTeammateMarkers:Boolean, a_showNeutralMarkers:Boolean):Void
{
	if (this.GetCreatingMarkers())
	{
		return;
	}

	while (extraMarkers.length)
	{
		var extraMarker = extraMarkers.pop();
		extraMarker.removeMovieClip();
	}

	// SkyUI?
	var markers:Array = this._markerList;
	if (markers == undefined)
	{
		// Vanilla then
		markers = this.Markers;
		if (markers == undefined)
		{
			return;
		}
	}

	var markersLen:Number = markers.length;

	var j:Number = 0;
	for (var i:Number = 0; i < markersLen; i++)
	{
		var createIconType = this.MarkerData[i * CREATE_STRIDE + CREATE_ICONTYPE];

		if (createIconType == 0)
		{
			// Replace with the extra marker icon
			var marker:MovieClip = markers[i];

			var extraMarkerType:String = ExtraMapMarker.IconTypes[this.ExtraMarkerData[j]];

			j++;

			var extraMarker = extraMarkerContainer.attachMovie(extraMarkerType, "ExtraMarker" + i, getNextHighestDepth());

			extraMarkers.push(extraMarker);
			markers[i] = extraMarker;

			marker.removeMovieClip();
		}
	}

	this.MarkerDescriptionObj._parent.swapDepths(getNextHighestDepth());
}
