class ObjectPanel extends React.Component {
  constructor (props) {
    super(props);
    this.state = {};
  }
  render () {
    return (
      <div className="panel panel-info object-panel">
        <div className="panel-heading row flex justify">
          <div className="row flex justify">
            <div className="col-flex">
              <h3 className="panel-title">title</h3>
            </div>
            <div className="col">
              <button
                className="btn btn-danger btn-xs"
                onClick={ this.props.manualRecognize }
              >人工识别</button>
            </div>
          </div>
        </div>
        <div className="panel-body">
          <div className="row flex justify middle">
            <div className="col">
              <div className="thumbnail object-origin-image">
                <img src={ this.props.image } />
              </div>
            </div>
            <div className="col">
              <i className="fa fa-arrows-h" />
            </div>
            <div className="col">
              <div className="thumbnail object-origin-image">
                <img src={ this.props.canny } />
              </div>
            </div>
          </div>
          {
            (this.props.score || []).map(score => {
              return (
                <div key={ score.uuid } className="media">
                  <div className="media-left media-middle">
                    <div className="thumbnail">
                      <img className="media-object" src={ score.image } />
                    </div>
                  </div>
                  <div className="media-body">
                    <h4 className="media-heading">{ score.uuid }</h4>
                    { score.faces.map(face => {
                      return (
                        <div className="media">
                          <div className="media-left media-middle">
                            <div className="thumbnail">
                              <img className="media-object" src={face.image} />
                            </div>
                          </div>
                          <div className="media-body">
                            <strong>contour:</strong>{ face.score.contour }<br />
                            <strong>hist:</strong>{ face.score.hist }<br />
                          </div>
                        </div>
                      );
                    })}
                  </div>
                </div>
              );
            })
          }


          <div className="row flex">
            <div className="col">
              <div className="thumbnail object-origin-image">
                <img src={ this.props.shapeImage } />
              </div>
            </div>
          </div>
        </div>
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
    this.socket = window.io.connect('http://localhost:8123/');
    this.socket
      .on('initItems', (items) => {
        console.log('init items', items);
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
      .on('score', this.onScore.bind(this));
  }
  getNextFrame () {
    this.socket.emit('getNextFrame');
  }
  onObjectFound (objects) {
    console.log('objects', objects);
    this.setState({ objects });
  }
  onFrame (frame) {
    this.setState({ frame });
  }
  onScore (data) {
    var obj = _.find(this.state.objects, obj=>obj.uuid == data.uuid);
    obj.score = data.score;

    this.setState({
      objects: this.state.objects
    });
  }
  manualRecognize (objUUID, itemUUID) {
    this.socket.emit('manualRecognize', {
      obj: objUUID,
      item: itemUUID
    });
  }
  render () {
    return (
      <div className="container">
        <div className="page-header" id="header">
          <nav className="navbar navbar-light bg-faded">
            <button className="navbar-toggler hidden-sm-up" type="button" data-toggle="collapse" data-target="#navbar-header" aria-controls="navbar-header" aria-expanded="false" aria-label="Toggle navigation"></button>
              <div className="collapse navbar-toggleable-xs" id="navbar-header">
                <a className="navbar-brand" href="#">Navbar</a>
                  <ul className="nav navbar-nav">
                    <li className="nav-item active">
                      <a className="nav-link" href="#">Home <span className="sr-only">(current)</span></a>
                    </li>
                    <li className="nav-item">
                      <a className="nav-link" href="#">Features</a>
                    </li>
                    <li className="nav-item">
                      <a className="nav-link" href="#">Pricing</a>
                    </li>
                    <li className="nav-item">
                      <a className="nav-link" href="#">About</a>
                    </li>
                  </ul>
                  <form className="form-inline float-xs-right">
                    <input className="form-control" type="text" placeholder="Search" />
                    <button className="btn btn-outline-success" type="submit">Search</button>
                  </form>
                </div>
              </nav>
        </div>
        <div className="row">
          <div className="col-8">
          <button
            type="button"
            className="btn btn-primary"
            onClick={ this.getNextFrame.bind(this) }
          >next</button>
          { this.state.frame &&
            <img src={ this.state.frame.image } className="video-capture"/>
          }
          </div>
          <div className="col-4">
          {
            this.state.objects.map((obj) =>
              <ObjectPanel
                image={ obj.image }
                shapeImage={ obj.shapeImage }
                canny={ obj.canny }
                key={ obj.uuid }
                score={ obj.score }
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