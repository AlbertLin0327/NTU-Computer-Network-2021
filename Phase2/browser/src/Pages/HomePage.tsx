import React from "react";
import { Navigate } from "react-router-dom";
import { Container, Table, Form, Row, Col, Button } from 'react-bootstrap';
import { NetworkServices } from "./MessageService";
import { setCookie, getCookie } from "../Utils/cookie";

interface HomePageState {
  Name: string | undefined;
  redirect: string | undefined;
  Friends: string[] | undefined;
  Newfriend: string | undefined;
}

class HomePage extends React.Component<{}, HomePageState> {
  constructor(props: any) {
    super(props);
    this.state = { Name: undefined, redirect: undefined, Friends: undefined, Newfriend: undefined };
  }

  componentDidMount = async () => {
    const Name = getCookie("name"); 
    var friends: string | undefined = await NetworkServices.Login(Name);
    var friendsList: string[] | undefined;
    if (friends !== undefined) {
        friendsList = friends.replace('[','').replace(']','').slice(0, -1).split(',');
        console.log('Hi', Name, friendsList);
    }

    if(friends){
      this.setState({ Friends: friendsList });
    }
    this.setState({Name: Name});
  }

  Updatedata = async () => {
    const Name = this.state.Name;
    if(Name) 
      var friends: string | undefined = await NetworkServices.Login(Name);
    var friendsList: string[] | undefined;
    if (friends !== undefined) {
        friendsList = friends.replace('[','').replace(']','').slice(0, -1).split(',');
        console.log('Hi', Name, friendsList);
    }

    if(friends){
      this.setState({ Friends: friendsList });
    }
    this.setState({Name: Name});
  }
  
  onSwitchpage = async (friend: string) => {
    if(this.state.Name && friend){
      setCookie("name", this.state.Name, 10);
      setCookie("sender", this.state.Name, 10);
      setCookie("receiver", friend, 10);
      this.setState({redirect: '/chat'});
    }
  }

  handleChange = (event: any) => {
    this.setState({Newfriend: event.target.value});
  }

  onSubmit = async () => {
    if(this.state.Name && this.state.Newfriend){
      console.log(this.state.Name, this.state.Newfriend);
      try {
        await NetworkServices.Addfriend(this.state.Name, this.state.Newfriend);
      } catch (e) {
        console.log(e);
      }
      await this.Updatedata();
    }
  }

  onItemdelete = async (friend: string) => {
    // console.log(this.state.Name, friend);
    if(this.state.Name){
      console.log(this.state.Name, friend);
      try {
        await NetworkServices.Deletefriend(this.state.Name, friend);

      } catch (e) {
        console.log(e);
      }
      await this.Updatedata();
    }
  } 

  render(){
    const Name = this.state.Name;
    const friends = this.state.Friends;
    if (this.state.redirect) {
      return <Navigate to={this.state.redirect} />;
    }
    if(friends === undefined){
      return <></>
    }
    return (
    <>
      <Container fluid>
        <Row>
          <h2 className="px-2 py-2">Welcome {Name}!</h2>
        </Row>
        <Row>
          <Col>
            <Form>
              <Form.Group controlId="exampleForm.ControlInput1">
                <Form.Control 
                  type="string" 
                  placeholder="Add friend?"
                  onChange={(e) => this.handleChange(e)}
                />
              </Form.Group>
            </Form>
          </Col>
          <Col> 
            <Button variant="outline-primary" onClick={() => this.onSubmit()}>Submit</Button>
          </Col>
        </Row>
        <Row>
          <Table className="table-hover table-striped">
            <thead>
              Friendlist
            </thead>
            <tbody>
            {friends.map((friend) => {
                return (
                  <tr key={friend}>
                    <td>{friend}</td>
                    <td>
                      {" "}
                      <Button
                        size="sm"
                        variant="info"
                        onClick={async () => await this.onSwitchpage(friend)}
                      >
                        {" "}
                        Chat{" "}
                      </Button>
                    </td>
                    <td>
                      {" "}
                      <Button
                        size="sm"
                        variant="danger"
                        onClick={async () => await this.onItemdelete(friend)}
                      >
                        {" "}
                        Delete{" "}
                      </Button>
                    </td>
                  </tr>
                );
              })}
            </tbody>
          </Table>
        </Row>
      </Container>
    </>
    );
  }
  
}

export default HomePage;
