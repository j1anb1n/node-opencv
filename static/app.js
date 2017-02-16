var socket = window.io.connect('http://localhost:8123/');

class Nav extends React.Component {
    constructor (props) {
        super(props);
        this.state = {};
    }
    render () {
      return (
        <nav className="navbar navbar-light bg-faded">
          <button className="navbar-toggler hidden-sm-up" type="button" data-toggle="collapse" data-target="#navbar-header" aria-controls="navbar-header" aria-expanded="false" aria-label="Toggle navigation"></button>
          <div className="collapse navbar-toggleable-xs" id="navbar-header">
            <a className="navbar-brand" href="#">Lego brick recognition</a>
            <ul className="nav navbar-nav">
              <li className="nav-item active">
                <a className="nav-link" href="#">Home <span className="sr-only">(current)</span></a>
              </li>
            </ul>
            <form className="form-inline float-xs-right">
              <input className="form-control" type="text" placeholder="Search" />
              <button className="btn btn-outline-success" type="submit">Search</button>
            </form>
          </div>
        </nav>
      );
    }
}

class RotatedRect extends React.Component {
  getStartPoint() {
    var {points} = this.props;
    if (this.props.angle < 0) {
      return _.min(points, p => p.x);
    } else {
      return _.min(points, p => p.x * p.y);
    }
  }
  render () {
    var { size, scale } = this.props;
    var { width, height } = size;
    var point = this.getStartPoint();
    var top = (point.x) * scale;
    var left = (point.y) * scale;

    return (
      <div className="rect"
        onClick={ this.props.onClick }
        style={{
          borderColor: this.props.color,
          height, width,
          transform: `translate(${top}px, ${left}px) rotate(${this.props.angle}deg) scale(${scale})`
        }}
      ></div>
    );
  }
}

class ObjectPanel extends React.Component {
  constructor (props) {
    super(props);
    this.state = {
      showPopup: false
    };
    this.onBodyClick = (e) => {
      if (!$.contains(this.refs.root, e.target)) {
        this.setState({
          showPopup: false
        });
      }
    };
  }
  showPopup () {
    this.setState({
      showPopup: true
    });
  }
  componentDidMount () {
    $('body').on('click', this.onBodyClick);
  }
  componentWillUnmount() {
    $('body').off('click', this.onBodyClick);
  }
  render () {
    var { rect, score, canny, shapeImage, match } = this.props.object;
    var color = 'green';
    if (typeof match === 'undefined') {
      color = 'blue';
    } else if (!match) {
      color = 'red';
    }
    console.log(score);

    return (
      <div ref="root">
        <RotatedRect
          color={ color }
          points={ rect.points }
          size={ rect.size }
          angle={ rect.angle }
          scale={ this.props.scale }
          onClick={ this.showPopup.bind(this) }
        />
        { this.state.showPopup &&
          <div className="popover popover-right">
            <h3 className="popover-title">
            { match ?
              match.uuid :
              <button
                className="btn btn-outline-success"
                onClick={ () => {
                  this.setState({
                    showPopup: false
                  });
                  this.props.manualRecognize();
                }}
              >手动识别</button>
            }
            </h3>
            <div className="popover-content">
            { match ?
              <div className="images row flex justify">
                <div className="col-3">
                  <img src={ match.image } />
                </div>
                <div className="col-3">
                  <img src={ canny } />
                </div>
                <div className="col-3">
                  <img src={ shapeImage } />
                </div>
              </div> :
              <div className="row flex justify">
                <div className="col-6">
                  <img src={ canny } />
                </div>
                <div className="col-6">
                  <img src={ shapeImage } />
                </div>
              </div>
            }
            </div>
          </div>
        }
      </div>
    );
  }
}

class ManualItemSetter extends React.Component {
  constructor (props) {
    super(props);
    this.state = {};
  }
  render () {
    return (
      <div className="mask pop-up manual-item-setter"
        onClick={(e) => {
          if (e.target == e.currentTarget) {
            this.props.onClose();
          }
        }}
      >
        <div className="content">
        { this.props.items.map((item, i) => {
          return (
            <div className="item" key={i}>
              <div className="thumbnail">
                <img src={ item.image } />
              </div>
              <button className="btn btn-primary"
                onClick={ () => {
                  this.props.onClose(item.uuid);
                }}
              >设置</button>
            </div>
          );
        })}
        </div>
      </div>
    );
  }
}

class SideBar extends React.Component {
  constructor (props) {
    super(props);
  }
  render () {
    return (
      <div className="sidebar">
        <div className="sidebar-header">

        </div>
      </div>
    )
  }
}

class App extends React.Component {
  constructor (props) {
    super(props);
    this.state = {
      connected: false,
      objects: [],
      frame: null,
      items: [],
      showManualItemSetter: false,
      objUUID: null
    };
  }
  componentDidMount () {
    socket
      .on('initItems', (items) => {
        this.setState({
          items
        });
      })
      .on('connect', () => {
        this.setState({
          connected: true
        });
      })
      .on('disconnect', () => {
        this.setState({
          connected: false
        });
      })
      .on('objectFound', this.onObjectFound.bind(this))
      .on('frame', this.onFrame.bind(this))
      .on('match', this.onMatch.bind(this));
  }
  getNextFrame () {
    socket.emit('getNextFrame');
  }
  onObjectFound (objects) {
    this.setState({ objects });
  }
  onFrame (frame) {
    this.setState({ frame });
  }
  onMatch (data) {
    var obj = _.find(this.state.objects, obj=>obj.uuid == data.uuid);
    obj.score = data.score;
    obj.match = data.match;

    this.setState({
      objects: this.state.objects
    });
  }
  manualRecognize (objUUID, itemUUID) {
    socket.emit('manualRecognize', {
      obj: objUUID,
      item: itemUUID
    });
  }
  render () {
    console.log(this.state.objects);

    return (
      <div className="container">
        <div className="page-header" id="header">
          <Nav />
          <button
            type="button"
            className="btn btn-primary"
            onClick={ this.getNextFrame.bind(this) }
          >next</button>
        </div>
        <div className="row">
          <div className="col-8">
            <div className="video">
              { this.state.frame &&
                <img src={ this.state.frame.image } className="video-capture"/>
              }
              {
                this.state.objects.map((obj) =>
                  <ObjectPanel
                    object={ obj }
                    key={ obj.uuid }
                    scale={ 740 / this.state.frame.size[1] }
                    manualRecognize={ () => {
                      this.setState({
                        showManualItemSetter: true,
                        objUUID: obj.uuid
                      });
                    }}
                  />)
              }
            </div>
          </div>
          <div className="col-4">
            <SideBar />
          </div>
        </div>


        { this.state.showManualItemSetter &&
          <ManualItemSetter items={ this.state.items }
            onClose={ (uuid) => {
              if (uuid) {
                this.manualRecognize(this.state.objUUID, uuid);
              }

              this.setState({
                showManualItemSetter: false,
                objUUID: null
              });
            }}
          />
        }
      </div>
    );
  }
}

ReactDOM.render(
  <App />,
  document.getElementById('app')
);