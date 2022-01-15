import React from "react";
import { Navigate } from "react-router-dom";
import { Container, Card, Form, Row, Col, Button } from 'react-bootstrap';
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
    const friends: string[] | undefined = await NetworkServices.Login(Name);
    if(friends){
      this.setState({ Friends: friends });
    }
  }
  
  handleChange = (event: any) => {
    this.setState({Name: event.target.value});
  }

  onSubmit = async () => {
    if(this.state.Name){
      try {
        const friends: string[] | undefined = await NetworkServices.Login(this.state.Name);
      } catch (e) {
        console.log(e);
      }
      this.setState({redirect: '/home'});
      
    }
  }


  render(){
    const Name = this.state.Name;
    if (this.state.redirect) {
      return <Navigate to={this.state.redirect} />;
    }
    return (
    <>
      <Container fluid>
        <Row>
          <h2 className="px-2 py-2">Welcome {Name}!</h2>
        </Row>
        <Row>
          <Card className="px-2">
            <Card.Title>Please click on person's button to choose action</Card.Title>
            <Card.Body>
              <Row>
              <Col> 
                <Button variant="outline-primary" onClick={() => this.onSubmit()}>Submit</Button>
              </Col>
              </Row>
            </Card.Body>
          </Card>
        </Row>
      </Container>
    </>
    );
  }
  
}

export default HomePage;
