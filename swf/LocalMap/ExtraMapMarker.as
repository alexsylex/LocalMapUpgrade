import gfx.io.GameDelegate;

class LocalMap.ExtraMapMarker extends gfx.controls.Button
{
	static var TopDepth: Number = 0;
	static var IconTypes: Array = new Array("EnemyMarker", "HostileMarker", "GuardMarker", "DeadMarker", "TeammateMarker", "NeutralMarker");
	var HitAreaClip;
	var Index:Number;
	var TextClip:MovieClip;
	var _FadingIn:Boolean;
	var _FadingOut:Boolean;
	var state:String;
	var stateMap:Array;
	var textField:TextField;

	var Type:String;

	function MapMarker()
	{
		super();
		hitArea = HitAreaClip;
		textField = TextClip.MarkerNameField;
		textField.autoSize = "left";
		Index = -1;
		disableFocus = true;
		_FadingIn = false;
		_FadingOut = false;
		stateMap.release = ["up"];
	}

	function configUI(): Void
	{
		super.configUI();
		onRollOver = function ()
		{
		};
		onRollOut = function ()
		{
		};
	}

	function get FadingIn(): Boolean
	{
		return _FadingIn;
	}

	function set FadingIn(value: Boolean): Void
	{
		if (value != _FadingIn) {
			_FadingIn = value;
			if (_FadingIn) {
				_visible = true;
				gotoAndPlay("fade_in");
			}
		}
	}

	function get FadingOut(): Boolean
	{
		return _FadingOut;
	}

	function set FadingOut(value: Boolean): Void
	{
		if (value != _FadingOut) {
			_FadingOut = value;
			if (_FadingOut) {
				gotoAndPlay("fade_out");
			}
		}
	}

	function setState(state: String): Void
	{
		if (!_FadingOut && !_FadingIn) {
			super.setState(state);
		}
	}

	function MarkerRollOver(): Boolean
	{
		var overState: Boolean = false;
		setState("over");
		overState = state == "over";
		if (overState) {
			var topInstance: MovieClip = _parent.getInstanceAtDepth(LocalMap.ExtraMapMarker.TopDepth);
			if (undefined != topInstance) {
				topInstance.swapDepths(LocalMap.ExtraMapMarker(topInstance).Index);
			}
			swapDepths(LocalMap.ExtraMapMarker.TopDepth);
			GameDelegate.call("PlaySound", ["UIMapRollover"]);
		}
		return overState;
	}

	function MarkerRollOut(): Void
	{
		setState("out");
	}

	function MarkerClick(): Void
	{
		GameDelegate.call("MarkerClick", [Index]);
	}

}
