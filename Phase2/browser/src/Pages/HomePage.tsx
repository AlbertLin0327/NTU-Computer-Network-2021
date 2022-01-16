import React from "react";
import { Navigate } from "react-router-dom";
import { Container, Table, Form, Row, Col, Button } from 'react-bootstrap';
import { NetworkServices } from "./MessageService";
import { getCookie } from "../Utils/cookie";

interface HomePageState {
  Name: string | undefined;
  redirect: string | undefined;
  Friends: string[] | undefined;
}

class HomePage extends React.Component<{}, HomePageState> {
  constructor(props: any) {
    super(props);
    this.state = { Name: undefined, redirect: undefined, Friends: undefined };
  }

  componentDidMount = async () => {
    const Name = getCookie("name"); 
    // var friends: string[] | undefined = await NetworkServices.Login(Name);
    var friends: string[] = ["Hermes", "Dino", ];
    console.log(Name, friends === undefined ? "ii" : [0]);
    if(friends){
      this.setState({ Friends: friends });
    }
    this.setState({Name: Name});
  }
  
  // handleChange = (event: any) => {
  //   this.setState({Name: event.target.value});
  // }

  // onSubmit = async () => {
  //   if(this.state.Name){
  //     try {
  //       const friends: string[] | undefined = await NetworkServices.Login(this.state.Name);
  //     } catch (e) {
  //       console.log(e);
  //     }
  //     this.setState({redirect: '/home'});
      
  //   }
  // }

  onItemdelete = async (friend: string) => {
    console.log(this.state.Name, friend);
    if(this.state.Name){
      console.log(this.state.Name, friend);
      try {
        await NetworkServices.Deletefriend(this.state.Name, friend);
      } catch (e) {
        console.log(e);
      }
      
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
                        variant="danger"
                        onClick={async () => await this.onItemdelete(friend)}
                      >
                        {" "}
                        刪除{" "}
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
